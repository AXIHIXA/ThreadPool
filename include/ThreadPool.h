#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <any>
#include <condition_variable>
#include <queue>
#include <vector>


struct Uncopyable
{
    Uncopyable() = default;
    ~Uncopyable() = default;
    Uncopyable(const Uncopyable &) = delete;
    Uncopyable & operator=(const Uncopyable &) = delete;
    Uncopyable(Uncopyable &&) = default;
    Uncopyable & operator=(Uncopyable &&) = default;
};


class Worker : Uncopyable
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


class ThreadPool : public Uncopyable
{
public:
    ThreadPool() = default;
    ~ThreadPool() = default;

    void start(int numWorkers = 4);

    bool submit(const std::shared_ptr<Runnable> & task);

private:
    void workerFunc();

    std::vector<std::unique_ptr<Worker>> workers;

    std::mutex taskQueueMutex;
    std::condition_variable taskQueueNotFull;
    std::condition_variable taskQueueNotEmpty;
    std::queue<std::shared_ptr<Runnable>> taskQueue;

    bool shouldStop = false;
};


#endif  // THREADPOOL_H
