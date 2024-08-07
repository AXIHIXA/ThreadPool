#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "ThreadPool.h"


class Sum final : public Task
{
public:
    Sum(int start, int end) : start(start), end(end) {}
    ~Sum() override = default;

    std::any run() override
    {
        int ans = 0;

        for (int i = start; i < end; ++i)
        {
            ans += i;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        return ans;
    }

private:
    int start = 0;
    int end = 0;
};


int main()
{
    ThreadPool pool;
    pool.start();

    std::cout << std::boolalpha;
    std::cout << pool.submit(std::make_shared<Sum>(1, 100)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 200)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 300)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 400)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 500)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 600)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 700)) << '\n';
    std::cout << pool.submit(std::make_shared<Sum>(1, 800)) << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return EXIT_SUCCESS;
}
