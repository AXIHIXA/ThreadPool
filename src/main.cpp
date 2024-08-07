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

        for (int i = start; i <= end; ++i)
        {
            ans += i;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        return ans;
    }

    std::string str() override
    {
        return "Sum(" + std::to_string(start) + ", " + std::to_string(end) + ")";
    }

private:
    int start = 0;
    int end = 0;
};


std::unique_ptr<Result> calculateSum(ThreadPool & pool, int start, int end)
{
    return pool.submit(std::make_shared<Sum>(start, end));
}


int getSum(Result * res)
{
    if (!res)
    {
        return -1;
    }

    return std::any_cast<int>(res->get());
}


int main()
{
    ThreadPool pool;
    pool.start();

    std::vector<std::unique_ptr<Result>> results;
    results.reserve(12);

    for (int i = 1; i <= 12; ++i)
    {
        results.emplace_back(calculateSum(pool, 1, i * 10));
    }

    for (auto & r : results)
    {
        std::cout << getSum(r.get()) << '\n';
    }

    return EXIT_SUCCESS;
}
