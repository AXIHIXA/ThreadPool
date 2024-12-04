#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>


class Task
{
public:
    Task() = default;
    virtual ~Task() = 0;
    virtual void run() = 0;
};

Task::~Task() = default;


class ActualTask : public Task
{
public:
    ActualTask() = default;
    ~ActualTask() override = default;

    void run() override
    {
        printf("task\n");
    }
};


class Worker
{
public:
    explicit Worker(std::function<void ()> func) : func(std::move(func))
    {

    }

    void work()
    {
        std::thread t(func);
        t.detach();
    }

    std::function<void ()> func;
};


class ThreadPool
{
public:
    explicit ThreadPool(int numWorkers, int maxTasks) : numWorkers(numWorkers), maxTasks(maxTasks)
    {
        for (int i = 1; i <= numWorkers; ++i)
        {
            workers.emplace_back(std::make_unique<Worker>([this]() { workerFunc(); }));
        }

        for (auto & pw : workers)
        {
            pw->work();
        }
    }

    ~ThreadPool()
    {
        if (isRunning)
        {
            stop();
        }
    }

    void stop()
    {
        isRunning = false;
        notEmpty.notify_all();
        notFull.notify_all();
    }

    void submit(std::shared_ptr<Task> task)
    {
        std::unique_lock lock(mut);
        notFull.wait(lock, [this] { return !isRunning || taskQueue.size() < workers.size(); });

        if (!isRunning)
        {
            return;
        }

        taskQueue.push(task);
        notEmpty.notify_all();

        if (taskQueue.size() < maxTasks)
        {
            notFull.notify_all();
        }
    }

private:
    void workerFunc()
    {
        while (isRunning)
        {
            std::shared_ptr<Task> task;

            {
                std::unique_lock lock(mut);
                notEmpty.wait(lock, [this] { return !isRunning || !taskQueue.empty(); });

                if (!isRunning)
                {
                    return;
                }

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
                task->run();
            }
        }
    }

    std::queue<std::shared_ptr<Task>> taskQueue;
    std::vector<std::unique_ptr<Worker>> workers;

    std::mutex mut;
    std::condition_variable notEmpty;
    std::condition_variable notFull;

    std::atomic_bool isRunning = true;

    int numWorkers;
    int maxTasks;
};


int main(int argc, char * argv[])
{
    {
        ThreadPool pool(4, 10);

        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());
        pool.submit(std::make_shared<ActualTask>());

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    printf("exiting\n");

    return EXIT_SUCCESS;
}
