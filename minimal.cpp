#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>


class ThreadPool
{
public:
    explicit ThreadPool(const int numWorkers)
    {
        workers.reserve(numWorkers);

        for (int i = 0; i < numWorkers; ++i)
        {
            workers.emplace_back(&ThreadPool::workerFunc, this);
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lk(mut);
            shouldStop = true;
        }

        // Workers wake up when task queue not empty, or thread pool should stop.
        notEmpty.notify_all();
        
        // 特性         析构中 join                           构造中 detach
        // 资源安全性    高，确保所有线程资源正确回收             低，依赖操作系统的自动回收
        // 性能         可能阻塞，适合少量长生命周期线程          非阻塞，适合大量短生命周期线程
        // 线程管理      明确，线程生命周期与线程池绑定            不明确，线程可能在线程池销毁后继续运行
        // 异常处理      易于捕获和管理线程异常                  难以处理分离线程中的异常
        // 适合场景      高可靠性、长生命周期、明确资源管理的场景   高并发、短生命周期、轻量级任务的场景
        for (auto & worker : workers)
        {
            worker.join();
        }
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    template <typename F, typename ... Args>
    auto submit(F && f, Args && ... args) -> std::future<std::invoke_result_t<F, Args ...>>
    {
        using R = std::invoke_result_t<F, Args...>;

        auto pTask = std::make_shared<std::packaged_task<R ()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<R> fut = pTask->get_future();

        {
            std::lock_guard<std::mutex> lk(mut);

            if (shouldStop)
            {
                throw std::runtime_error("task submitted to a stopped ThreadPool");
            }

            tasks.emplace([pTask]() { (*pTask)(); });
        }

        notEmpty.notify_one();

        return fut;
    }

private:
    using Task = std::function<void ()>;

    void workerFunc()
    {
        while (true)
        {
            Task task;

            {
                std::unique_lock<std::mutex> lk(mut);

                notEmpty.wait(lk, [this]
                {
                    return shouldStop || !tasks.empty();
                });

                if (shouldStop && tasks.empty())
                {
                    break;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
        }
    }

    std::vector<std::thread> workers;

    std::mutex mut;
    std::condition_variable notEmpty;
    std::queue<Task> tasks;

    std::atomic_bool shouldStop = false;
};


int main(int argc, char * argv[])
{
    std::future<int> f1, f2, f3, f4;

    {
        ThreadPool pool(4);

        auto add = [](int a, int b)
        {
            return a + b;
        };

        f1 = pool.submit(add, 0, 1);
        f2 = pool.submit(add, 0, 2);
        f3 = pool.submit(add, 0, 3);
        f4 = pool.submit(add, 0, 4);
    }

    std::cout << f1.get() << ' ' << f2.get() << ' ' << f3.get() << ' ' << f4.get() << '\n';

    return EXIT_SUCCESS;
}
