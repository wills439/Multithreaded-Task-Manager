#include<iostream>
#include<vector>
#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<condition_variable>

struct Task{
    std::function<void()> task;
};

class ThreadPool{
    std::vector<std::thread> workers;
    std::queue<Task> taskQeueu;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
    void WorkerLoop(){
        while(1){
            Task currentTask;
            {
                std::unique_lock<std::mutex>guard(mtx);
                cv.wait(guard, [this]{return !taskQeueu.empty() || stop;});
                
                if(stop && taskQeueu.empty())
                    return;
                
                currentTask = taskQeueu.front();    
                taskQeueu.pop();
            }    
            currentTask.task();
        }
    }

    public:
        ThreadPool(size_t workerNum){
            for(size_t i = 0; i < workerNum; i ++)
                workers.emplace_back(&ThreadPool::WorkerLoop, this);
        }
    
        void AddTaskToQueue(Task task){
            std::lock_guard<std::mutex>guard(mtx);
            taskQeueu.push(task);
            cv.notify_one();
        }

        ~ThreadPool(){
            {
                std::lock_guard<std::mutex> guard(mtx);
                stop = true;
            }
            cv.notify_all();
            for(size_t i = 0; i < workers.size(); i++)
                workers[i].join();
        }
};

//intentional o(n) prime computation algorithm
void ComputePrimes(size_t end){
    std::vector<int> primes;
    for(size_t i = 2; i < end; i++){
        int isPrime = 1;
        for(size_t j = i; j > 0; j--)
            if(i%j == 0 && i != j && j != 1)
                isPrime = 0;
        if(isPrime){
            primes.push_back(i);
            std::cout << "Primes: " << i << "\n";
        }
    }
}

int main(){
    ThreadPool pool(8);

    const int NUM_TASKS = 10;
    std::vector<Task> primesTasks;
    for(int i = 0; i < NUM_TASKS; i++){
        Task primesTask;
        primesTask.task = [](){ComputePrimes(10000);};
        primesTasks.push_back(primesTask);
    }

    for(Task task:primesTasks)
        pool.AddTaskToQueue(task);
    
    return 0;
}