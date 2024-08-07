#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <any>
#include <condition_variable>
#include <queue>
#include <unordered_map>


class Worker
{
public:
    Worker(int handle, std::function<void ()> func);
    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker & operator=(const Worker &) = delete;
    Worker(Worker &&) = default;
    Worker & operator=(Worker &&) = default;

    void work() const;

    int getHandle() const;

private:
    int handle = 0;
    std::function<void ()> func;
};


class Task
{
public:
    Task() = default;
    virtual ~Task() = 0;

    virtual std::any run() = 0;
    void exec();
};


class ThreadPool
{
public:
    enum Mode
    {
        kFixed = 1,
        kCached = 2,
    };

    ThreadPool();
    ThreadPool(Mode mode, int minNumWorkers, int maxNumWorkers);

    ~ThreadPool() = default;

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool & operator=(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;

    void start();

    bool submit(const std::shared_ptr<Task> & task);

private:
    void workerFunc();

    void initWorkers();

    std::unordered_map<int, std::unique_ptr<Worker>> workers;

    std::mutex mut;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
    std::queue<std::shared_ptr<Task>> taskQueue;

    bool running = false;

    Mode mode = kFixed;
    int minNumWorkers = 4;
    int maxNumWorkers = 4;
};


#endif  // THREADPOOL_H
