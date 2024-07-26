#ifndef ANY_H
#define ANY_H

#include <exception>
#include <memory>


// Prototype implementation of std::any w/o metaprogramming tricks involving value categories.
class Any
{
public:
    template <typename T>
    explicit Any(T t) : pBase(std::make_unique<Derived<T>>(t)) {}

    template <typename T>
    T cast()
    {
        auto p = dynamic_cast<Derived<T> *>(pBase.get());

        if (!p)
        {
            throw std::invalid_argument("bad Any::cast");
        }

        return p->data;
    }

private:
    struct Base
    {
        virtual ~Base() = default;
    };

    template <typename T>
    struct Derived : Base
    {
        explicit Derived(T t) : data(t) {}
        ~Derived() override = default;

        T data;
    };

    std::unique_ptr<Base> pBase;
};


#endif  // ANY_H
