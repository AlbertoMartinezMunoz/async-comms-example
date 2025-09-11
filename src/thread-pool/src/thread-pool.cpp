#include <thread-pool/thread-pool.hpp>

thread_pool::thread_pool(size_t size = std::thread::hardware_concurrency()) : stop(false) {
    for (size_t i = 0; i < size; ++i) {
        pool.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    cv.wait(lock, [this] { return !tasks.empty() || stop; });
                    if (stop) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

thread_pool::~thread_pool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cv.notify_all();

    for (auto &thread : pool) {
        thread.join();
    }
}

void thread_pool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::move(task));
    }
    cv.notify_one();
}
