#include<iostream>
#include<vector>
#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<condition_variable>

enum class Status{Pending, Complete, Active};
enum class Priority{Low, Medium, High};

struct Task{
    std::function<void()> task;
    Status taskStatus = Status::Pending;
    Priority taskPriority;
};

class ThreadPool{
    std::vector<std::thread> workers;
    std::queue<Task> taskQeueu;
    std::mutex queueMtx;
    void WorkerLoop(){
        Task currentTask;
        {
            std::lock_guard<std::mutex>guard(queueMtx);
            currentTask = taskQeueu.front();
            taskQeueu.pop();
        }
        
        currentTask.task();
    }

    private:
        void SendTaskToWorker(){
            workers.emplace_back(&ThreadPool::WorkerLoop, this);
        }

    public:
        void AddTaskToQueue(Task task){
            std::lock_guard<std::mutex>guard(queueMtx);
            taskQeueu.push(task);
            SendTaskToWorker();
        }

        ~ThreadPool(){
            for(size_t i = 0; i < workers.size(); i++)
                workers[i].join();
        }
};

void ComputePrimes(size_t end, int id){
    std::vector<int> primes;
    for(size_t i = 2; i < end; i++){
        int isPrime = 1;
        for(size_t j = i; j > 0; j--)
            if(i%j == 0 && i != j && j != 1)
                isPrime = 0;
        if(isPrime){
            primes.push_back(i);
            std::cout << "Primes " << id << ": " << i << "\n";
        }
    }
}

int main(){
    ThreadPool pool;

    Task primes_one;
    primes_one.task = [](){ComputePrimes(10000,1);};

    Task primes_two;
    primes_two.task = [](){ComputePrimes(10000,2);};

    pool.AddTaskToQueue(primes_one);
    pool.AddTaskToQueue(primes_two);
    
    return 0;
}