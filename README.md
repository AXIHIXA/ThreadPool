# ThreadPool

- Sample thread pool with C++17 semantics.
- Prototype implementation of `std::any` and `std::future` to catch return values of async tasks.
- Shows sample usage of STL containers and concurrency support primitives in this project. 
- A self-contained minimum example is available in `minimal.cpp`. 

## PARADIGM: Notify, Then Unlock

- **Correctness**: Avoids potential deadlocks caused by "lost wakeups".
  - The "unlock, then notify" paradigm may cause "lost wakeups" and deadlocks!
  - If the consumer thread is scheduled before it waits on the condition variable (i.e., when it's evaluating the predicate),
  - then, when the producer thread notifies, no thread is currently waiting on the condition variable!
    - This notification will be lost.
    - The producer will wait forever (deadlock)!
- **Efficiency**: Avoids unnecessary context switches and resource races, enabling a seamless thread concatenation.
  - If you signal before you unlock, a good implementation will know that your signal **cannot possibly render any thread ready-to-run** because the mutex is held by the calling thread and any thread affects by the condition variable necessarily cannot make forward progress without the mutex. This permits a significant optimization (often called "wait morphing") that is not possible if you unlock first.
- Reference: [StackOverflow](https://stackoverflow.com/questions/52503361/unlock-the-mutex-after-condition-variablenotify-all-or-before)
