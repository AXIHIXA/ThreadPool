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

    std::unique_ptr<Result> r1 = calculateSum(pool, 1, 100);
    std::unique_ptr<Result> r2 = calculateSum(pool, 1, 200);
    std::unique_ptr<Result> r3 = calculateSum(pool, 1, 300);
    std::unique_ptr<Result> r4 = calculateSum(pool, 1, 400);
    std::unique_ptr<Result> r5 = calculateSum(pool, 1, 500);
    std::unique_ptr<Result> r6 = calculateSum(pool, 1, 600);
    std::unique_ptr<Result> r7 = calculateSum(pool, 1, 700);
    std::unique_ptr<Result> r8 = calculateSum(pool, 1, 800);

    std::cout << getSum(r1.get()) << '\n';
    std::cout << getSum(r2.get()) << '\n';
    std::cout << getSum(r3.get()) << '\n';
    std::cout << getSum(r4.get()) << '\n';
    std::cout << getSum(r5.get()) << '\n';
    std::cout << getSum(r6.get()) << '\n';
    std::cout << getSum(r7.get()) << '\n';
    std::cout << getSum(r8.get()) << '\n';

    return EXIT_SUCCESS;
}
