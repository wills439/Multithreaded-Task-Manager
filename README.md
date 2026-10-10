# C++ Thread Pool

A small, dependency-free thread pool in C++ built on `std::thread`, `std::mutex`, and `std::condition_variable`. Tasks are queued as `std::function<void()>` objects and executed concurrently by a fixed set of worker threads.

The demo in `main.cpp` queues 10 CPU-heavy jobs (a deliberately naive prime search up to 10,000) and runs them across 8 workers.

## Features

- **Fixed-size worker pool**: threads are created once and reused for every task.
- **Thread-safe task queue**: a single mutex guards the queue and the shutdown flag.
- **No busy-waiting**: idle workers sleep on a condition variable and are woken only when work arrives.
- **Parallel execution**: the queue lock is released before a task runs, so tasks execute concurrently.
- **Graceful shutdown (RAII)**: the destructor signals workers, lets them drain the remaining queue, and joins every thread.

## How it works

```
main thread                       worker threads (x8)
-----------                       -------------------
AddTaskToQueue(task)              lock mutex
  lock mutex                      cv.wait(until queue non-empty OR stop)
  push task                       if stop and queue empty -> exit
  unlock                          pop task
  cv.notify_one() --------------> unlock mutex
                                  run task (no lock held)
~ThreadPool()                     loop
  lock, stop = true, unlock
  cv.notify_all()
  join all workers
```

### Design notes

- **Predicate-based wait.** `cv.wait(lock, pred)` re-checks the condition after every wake-up, which protects against spurious wake-ups and against another worker taking the task first.
- **One mutex for related state.** `taskQueue` and `stop` are both read in the wait predicate, so they share one mutex.
- **`stop` is set under the mutex.** This prevents a lost wake-up, where a worker checks the predicate, the destructor sets `stop` and notifies, and the worker then sleeps forever.
- **Lock released before joining.** The destructor sets `stop` in a scoped lock and joins only after releasing it. Joining while holding the mutex deadlocks, because woken workers must re-acquire that mutex to exit.
- **Drain on shutdown.** Workers exit only when `stop` is set *and* the queue is empty, so queued tasks are never dropped.

## Build and run

Requires a C++11-or-newer compiler and pthreads.

```bash
g++ -std=c++17 -O2 -pthread main.cpp -o threadpool
./threadpool
```

## Usage

```cpp
ThreadPool pool(8);              // start 8 workers

Task t;
t.task = []{ /* your work here */ };
pool.AddTaskToQueue(std::move(t));

// leaving scope runs ~ThreadPool(): finishes queued tasks, joins threads
```

## Known limitations / ideas

- Tasks return nothing; returning results would need `std::future` / `std::packaged_task`.
- Plain FIFO queue; priority scheduling would need a `std::priority_queue`.
- `std::cout` output from concurrent tasks can interleave; protect it with its own mutex or collect results and print once.
- The prime search is intentionally naive to generate CPU load. Checking divisors up to `sqrt(n)` would be much faster.
- `AddTaskToQueue` is not guarded against being called after shutdown has begun.
