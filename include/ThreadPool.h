#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <any>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>


class UnCopyable
{
public:
    UnCopyable() = default;
    ~UnCopyable() = default;
    UnCopyable(const UnCopyable &) = delete;
    UnCopyable & operator=(const UnCopyable &) = delete;
    UnCopyable(UnCopyable &&) = default;
    UnCopyable & operator=(UnCopyable &&) = default;
};


class Worker : UnCopyable
{
public:
    explicit Worker(std::function<void ()> func);
    ~Worker() = default;

    void listen() const;

private:
    std::function<void ()> func;
};


class Runnable
{
public:
    Runnable() = default;
    virtual ~Runnable() = 0;

    virtual std::any run() = 0;

    void execute();
};


class ThreadPool : public UnCopyable
{
public:
    ThreadPool() = default;
    ~ThreadPool() = default;

    void start(int numWorkers = 4);

    bool submit(const std::shared_ptr<Runnable>&);

private:
    void threadFunc();

    std::vector<std::shared_ptr<Worker>> workers;

    std::mutex taskQueueMutex;
    std::condition_variable taskQueueNotFull;
    std::condition_variable taskQueueNotEmpty;
    std::queue<std::shared_ptr<Runnable>> taskQueue;
};



#endif  // THREADPOOL_H
