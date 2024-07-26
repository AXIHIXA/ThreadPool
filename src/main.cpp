#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadPool.h"


struct Task : Runnable
{
    std::any run() override
    {
        std::cout << "Starting " << std::this_thread::get_id() << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Ending " << std::this_thread::get_id() << '\n';
        return {};
    }
};


int main()
{
    ThreadPool pool;
    pool.start(2);

    std::cout << std::boolalpha;
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';
    std::cout << pool.submit(std::make_shared<Task>()) << '\n';


    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
