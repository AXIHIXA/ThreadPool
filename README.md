# ThreadPool

- Sample thread pool with C++17 semantics.
- Prototype implementation of `std::any` and `std::future` to catch return values of async tasks.
- Shows sample usage of STL containers and concurrency support primitives in this project. 
- A self-contained minimum example is available in `minimal.cpp`. 

## PARADIGM: Unlock, Then Notify

- Reference: [面试官：notify()和unlock()谁先谁后？答错这道C++并发“送命题”，offer秒飞。](https://mp.weixin.qq.com/mp/wappoc_appmsgcaptcha?poc_token=HKnGlmijv2r4-ewn0_xN5cjeJiXFlBRwcg6dJy5f&target_url=https%3A%2F%2Fmp.weixin.qq.com%2Fs%2Fv63Al0lWswT-PBAj8_Uzrg)
- **Correctness**: Avoids potential deadlocks caused by "lost wakeups".
  - The "notify, then unlock" paradigm may cause "lost wakeups" and deadlocks!
  - If the consumer thread is scheduled before it waits on the condition variable (i.e., when it's evaluating the predicate),
  - then, when the producer thread notifies, no thread is currently waiting on the condition variable!
    - This notification will be lost.
    - The producer will wait forever (deadlock)!
- **Efficiency**: Avoids unnecessary context switches and resource races, enabling a seamless thread concatenation.
