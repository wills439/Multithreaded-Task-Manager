#include<iostream>
#include<vector>
#include<thread>
#include<queue>
#include<functional>

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
    
    void WorkerLoop(){
        Task currentTask;
        currentTask = taskQeueu.front();

        currentTask.task();

        taskQeueu.pop();
    }

    private:
        void SendTaskToWorker(){
            workers.emplace_back(&ThreadPool::WorkerLoop, this);
        }

    public:
        void AddTaskToQueue(Task task){
            taskQeueu.push(task);
        }
};

void ComputePrimes(size_t end){
    std::vector<int> primes;
    for(size_t i = 2; i < end; i++){
        int isPrime = 1;
        for(size_t j = i; j > 0; j--)
            if(i%j == 0 && i != j && j != 1)
                isPrime = 0;
        if(isPrime)
            primes.push_back(i);
    }

    for(int i = 0; i < primes.size(); i++)
        std::cout << primes[i] << "\n";
}

int main(){
    ThreadPool pool;

    Task primes_one;
    primes_one.task = [](){ ComputePrimes(1000);};

    pool.AddTaskToQueue(primes_one);
    
    return 0;
}