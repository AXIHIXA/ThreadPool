#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <any>
#include <condition_variable>
#include <queue>
#include <unordered_map>


class Worker
{
public:
    Worker(int handle, std::function<void (int)> func);
    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker & operator=(const Worker &) = delete;
    Worker(Worker &&) = default;
    Worker & operator=(Worker &&) = default;

    void work() const;

    [[nodiscard]] int getHandle() const;

private:
    int handle = 0;
    int threadId = 0;
    std::function<void (int)> func;
};


class Result;


class Task
{
public:
    Task() = default;
    virtual ~Task() = 0;

    virtual std::any run() = 0;
    virtual std::string str();

    void exec();
    void setResult(Result * res);

private:
    Result * pRes = nullptr;
};


class Result
{
public:
    Result() = default;
    explicit Result(std::shared_ptr<Task> task);
    ~Result() = default;

    Result(const Result &) = delete;
    Result & operator=(const Result &) = delete;
    Result(Result &&) = delete;
    Result & operator=(Result &&) = delete;

    void setVal(const std::any & val);
    void setVal(std::any && val);

    std::any get();

private:
    bool ready = false;
    std::mutex mut;
    std::condition_variable cv;
    std::any val;

    // Save the reference count for the task in this result,
    // otherwise it will be destructed once popped out of the task queue!
    std::shared_ptr<Task> pTask = nullptr;
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

    std::unique_ptr<Result> submit(const std::shared_ptr<Task> & task);

private:
    static constexpr int kDefaultNumWorkers = 4;

    void initWorkers();

    void workerFunc(int handle);

    // Worker threads.
    std::unordered_map<int, std::unique_ptr<Worker>> workers;

    // // Fall-back observer to avoid deadlocks.
    // // Observes taskQueue in a separate thread.
    // std::unique_ptr<Worker> fallbackObserver;

    std::mutex mut;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
    std::queue<std::shared_ptr<Task>> taskQueue;

    std::atomic_bool running = false;

    Mode mode = kFixed;
    int minNumWorkers = kDefaultNumWorkers;
    int maxNumWorkers = kDefaultNumWorkers;
};


#endif  // THREADPOOL_H
