#include<iostream>
#include<vector>
#include<thread>
#include<queue>

enum class Status{Pending, Complete, Active};
enum class Priority{Low, Medium, High};

struct Task{
    void(*task)(void*);
    void* args;
    Status taskStatus = Status::Pending;
    Priority taskPriority;
};

class ThreadPool{
    std::vector<std::thread> workers;
    std::queue<Task> taskQeueu;
    
    void WorkerLoop(){
        Task currentTask;
        currentTask = taskQeueu.front();

        currentTask.task(nullptr);

        taskQeueu.pop();
    }

    public:
        ThreadPool(size_t threadNum){
            workers.resize(threadNum);
        }

        void AddTaskToQueue(Task task){
            taskQeueu.push(task);
        }
        
        void SendTaskToWorker(){
            workers.emplace_back(&ThreadPool::WorkerLoop, this);
        }
};

void ComputePrimes(void* args){
    //make read args void ptr
    
    std::vector<int> primes;
    for(size_t i = 2; i < args->end; i++){
        int isPrime = 1;
        for(size_t j = i; j > 0; j--){
            if(i%j == 0 && i != j && j != 1){
                isPrime = 0;
            }
        }

        if(isPrime)
            primes.push_back(i);
    }

    for(int i = 0; i < primes.size(); i++)
        std::cout << primes[i] << "\n";
}

int main(){
    ThreadPool pool(8);
    
    return 0;
}