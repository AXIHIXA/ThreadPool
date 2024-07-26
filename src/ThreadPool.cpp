#include <future>
#include <iostream>

#include "ThreadPool.h"


Worker::Worker(std::function<void ()> func) : func(std::move(func))
{

}


void Worker::listen() const
{
    std::thread t(func);
    t.detach();
}


Runnable::~Runnable() = default;


void Runnable::execute()
{
    run();
}


void ThreadPool::start(int numWorkers)
{
    workers.resize(numWorkers);
    shouldStop = false;

    for (auto & worker : workers)
    {
        worker = std::make_unique<Worker>([this] { workerFunc(); });
    }

    for (const auto & worker : workers)
    {
        worker->listen();
    }
}


bool ThreadPool::submit(const std::shared_ptr<Runnable> & task)
{
    std::unique_lock lock(taskQueueMutex);

    bool success = taskQueueNotFull.wait_for(
            lock,
            std::chrono::seconds(1),
            [this] { return taskQueue.size() < workers.size(); });

    if (!success)
    {
        return false;
    }

    taskQueue.push(task);
    taskQueueNotEmpty.notify_all();

    if (taskQueue.size() < workers.size())
    {
        taskQueueNotFull.notify_all();
    }

    lock.unlock();

    return true;
}


// Each worker executes this function in a detached thread.
// Each worker fetches a task from the task queue and then run it.
void ThreadPool::workerFunc()
{
    // std::cout << std::this_thread::get_id() << "begins\n";
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    while (!shouldStop)
    {
        std::shared_ptr<Runnable> task = nullptr;
        {
            std::unique_lock lock(taskQueueMutex);
            taskQueueNotEmpty.wait(lock, [this] { return !taskQueue.empty(); });
            task = taskQueue.front();
            taskQueue.pop();

            if (!taskQueue.empty())
            {
                taskQueueNotEmpty.notify_all();
            }

            taskQueueNotFull.notify_all();
        }

        if (task)
        {
            task->execute();
        }
    }

    // std::cout << std::this_thread::get_id() << "stops\n";
}


