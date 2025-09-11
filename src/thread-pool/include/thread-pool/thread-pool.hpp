#ifndef IOT_MICRO_FIRMWARE_SRC_THREAD_POOL_INCLUDE_THREAD_POOL_THREAD_POOL__H_
#define IOT_MICRO_FIRMWARE_SRC_THREAD_POOL_INCLUDE_THREAD_POOL_THREAD_POOL__H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class thread_pool {
  public:
    thread_pool(size_t size);
    ~thread_pool();

    void enqueue(std::function<void()> task);

  private:
    std::vector<std::thread> pool;
    std::queue<std::function<void()>> tasks;
    bool stop;
    std::mutex queue_mutex;
    std::condition_variable cv;
};

#endif // IOT_MICRO_FIRMWARE_SRC_THREAD_POOL_INCLUDE_THREAD_POOL_THREAD_POOL__H_
