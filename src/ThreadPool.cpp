#include <cassert>
#include <future>
#include <iostream>

#include "ThreadPool.h"


Worker::Worker(int handle, std::function<void ()> func) :
    handle(handle),
    func(std::move(func))
{

}


void Worker::work() const
{
    std::thread t(func);
    t.detach();
}


int Worker::getHandle() const
{
    return handle;
}


Task::~Task() = default;


void Task::exec()
{
    run();
}


ThreadPool::ThreadPool()
{
    initWorkers();
}


ThreadPool::ThreadPool(Mode mode, int minNumWorkers, int maxNumWorkers) :
    mode(mode),
    minNumWorkers(minNumWorkers),
    maxNumWorkers(maxNumWorkers)
{
    assert(mode == kFixed ? minNumWorkers == maxNumWorkers : minNumWorkers <= maxNumWorkers);
    initWorkers();
}


void ThreadPool::start()
{
    running = true;
}


bool ThreadPool::submit(const std::shared_ptr<Task> & task)
{
    std::unique_lock lock(mut);

    bool success = notFull.wait_for(
        lock,
        std::chrono::seconds(1),
        [this] { return taskQueue.size() < workers.size(); }
    );

    if (!success)
    {
        // Timeout.
        return false;
    }

    taskQueue.push(task);
    notEmpty.notify_all();

    if (taskQueue.size() < workers.size())
    {
        notFull.notify_all();
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

    while (running)
    {
        std::shared_ptr<Task> task = nullptr;
        {
            std::unique_lock lock(mut);
            notEmpty.wait(lock, [this] { return !taskQueue.empty(); });
            task = taskQueue.front();
            taskQueue.pop();

            if (!taskQueue.empty())
            {
                notEmpty.notify_all();
            }

            notFull.notify_all();
        }

        if (task)
        {
            task->exec();
        }
    }

    // std::cout << std::this_thread::get_id() << "stops\n";
}


void ThreadPool::initWorkers()
{
    for (int i = 1; i <= minNumWorkers; ++i)
    {
        workers.emplace(i, std::make_unique<Worker>(i, [this] { workerFunc(); }));
    }

    for (auto & [handle, pw] : workers)
    {
        pw->work();
    }
}
