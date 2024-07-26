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


// // Prototype implementation of std::any w/o metaprogramming tricks involving value categories.
// class Any : Uncopyable
// {
// public:
//     template <typename T>
//     explicit Any(T t) : pBase(std::make_unique<Derived<T>>(t)) {}
//
//     template <typename T>
//     T cast()
//     {
//         auto p = dynamic_cast<Derived<T> *>(pBase.get());
//
//         if (!p)
//         {
//             throw std::bad_any_cast();
//         }
//
//         return p->data;
//     }
//
// private:
//     struct Base
//     {
//         virtual ~Base() = default;
//     };
//
//     template <typename T>
//     struct Derived : Base
//     {
//         explicit Derived(T t) : data(t) {}
//         ~Derived() override = default;
//
//         T data;
//     };
//
//     std::unique_ptr<Base> pBase;
// };


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
