# ThreadPool

- Sample thread pool with C++17 semantics.
- Prototype implementation of `std::any` and `std::future` to catch return values of async tasks.
- Shows sample usage of STL containers and concurrency support primitives in this project. 
- A self-contained minimum example is available in `minimal.cpp`. 


## PARADIGM: Notify, Then Unlock vs Unlock, Then Notify

- Reference: [StackOverflow](https://stackoverflow.com/questions/52503361/unlock-the-mutex-after-condition-variablenotify-all-or-before)
  - **Common practice: Unblock first, then notify**.
    - [https://en.cppreference.com/w/cpp/thread/condition_variable/notify_all](https://en.cppreference.com/w/cpp/thread/condition_variable/notify_all)
    - [Back to Basics: Concurrency - Arthur O'Dwyer - CppCon 2020 @36:18](https://www.youtube.com/watch?v=F6Ipn7gCOsY&t=5s)
  - Performance-wise, unlock first is better; it avoids consumer thread to be waked up and immediately blocked.
  - Correctness-wise, unlock first requires consumer thread to check for the condition before sleeping. Otherwise, there will be *sleep paralysis* (cv counterpart fot for mutex deadlocks).
    - only when all 3 points are violated ( 1. condvar associated with several separate conditions and/or didn't check it before waiting again; 2. signal/notify only one thread when there're more than one other threads using the condvar; 3. unlock before notify creating a window for race condition ), the "sleep paralysis" would occur
