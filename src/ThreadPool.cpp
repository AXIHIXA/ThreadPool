#include <cassert>
#include <future>
#include <iostream>

#include "ThreadPool.h"


Worker::Worker(int handle, std::function<void (int)> func) :
    handle(handle),
    func(std::move(func))
{

}


void Worker::work() const
{
    std::thread t(func, handle);
    t.detach();
}


int Worker::getHandle() const
{
    return handle;
}


Task::~Task() = default;


std::string Task::str()
{
    return "Task";
}


void Task::exec()
{
    if (pRes)
    {
        pRes->setVal(run());
    }
}


void Task::setResult(Result * res)
{
    pRes = res;
}


Result::Result(std::shared_ptr<Task> task) : pTask(std::move(task))
{
    pTask->setResult(this);
}


void Result::setVal(const std::any & val)
{
    std::lock_guard g(mut);
    ready = true;
    this->val = val;
    cv.notify_all();
}


void Result::setVal(std::any && val)
{
    std::lock_guard g(mut);
    ready = true;
    this->val = std::move(val);
    cv.notify_all();
}


std::any Result::get()
{
    std::unique_lock lock(mut);
    cv.wait(lock, [this] -> bool { return ready; });
    return std::move(val);
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

    for (auto & [handle, pw] : workers)
    {
        pw->work();
    }
}


std::unique_ptr<Result> ThreadPool::submit(const std::shared_ptr<Task> & task)
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
        return nullptr;
    }

    taskQueue.push(task);

    notEmpty.notify_all();

    if (taskQueue.size() < maxNumWorkers)
    {
        notFull.notify_all();
    }

    return std::make_unique<Result>(task);
}


void ThreadPool::initWorkers()
{
    // Workers.
    for (int i = 1; i <= minNumWorkers; ++i)
    {
        workers.emplace(
            i,
            std::make_unique<Worker>(i, [this](int handle) { workerFunc(handle); })
        );
    }

    // // Fall-back task queue observer to avoid deadlocks.
    // // When cv.notify is called, workers might not be idle waiting yet;
    // // when they finally wait on the a cv, cv.notify is already called.
    // // Without this fall-back observer, these workers will never be notified!
    // fallbackObserver = std::make_unique<Worker>(0, [this](int handle)
    // {
    //     std::lock_guard g(mut);
    //
    //     if (!taskQueue.empty())
    //     {
    //         notEmpty.notify_all();
    //     }
    //
    //     if (taskQueue.size() < maxNumWorkers)
    //     {
    //         notFull.notify_all();
    //     }
    // });
}


// Each worker executes this function in a detached thread.
// Each worker fetches a task from the task queue and then run it.
void ThreadPool::workerFunc(int handle)
{
    // std::string ss = "Worker #" + std::to_string(handle) + " begins\n";
    // std::cout << ss;
    // std::flush(std::cout);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

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
