#include<iostream>
#include<vector>
#include<thread>
#include<queue>

enum class Status{Pending, Complete, Active};
enum class Priority{Low, Medium, High};

struct Task{
    void(*task)();
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


    public:
        ThreadPool(size_t threadNum){
            workers.resize(threadNum);
        }

        void AddTaskToQueue(Task task){
            taskQeueu.push(task);
        }
        
        void SendTaskToWorker(){
            workers.emplace_back(WorkerLoop);
        }
};

int main(){
    ThreadPool pool(8);
    
    return 0;
}