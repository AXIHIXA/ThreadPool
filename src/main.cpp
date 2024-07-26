#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadPool.h"


struct Task final : Runnable
{
    std::any run() override
    {
        std::cout << "Starting " << std::this_thread::get_id() << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Ending " << std::this_thread::get_id() << '\n';
        return {};
    }
};


int main()
{
    ThreadPool tp;
    tp.start();

    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());
    // tp.submit(std::make_shared<Task>());

    std::this_thread::sleep_for(std::chrono::seconds(4));
    return 0;
}
