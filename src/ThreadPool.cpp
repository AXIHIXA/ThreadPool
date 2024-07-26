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
    workers.resize(numWorkers, std::make_shared<Worker>([this] { threadFunc(); }));

    for (const auto & worker : workers)
    {
        worker->listen();
    }
}


bool ThreadPool::submit(const std::shared_ptr<Runnable> & task)
{

}


void ThreadPool::threadFunc()
{
    std::cout << std::this_thread::get_id() << "begins\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << std::this_thread::get_id() << "stops\n";
}


