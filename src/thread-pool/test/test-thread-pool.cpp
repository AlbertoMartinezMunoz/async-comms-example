#include <thread-pool/thread-pool.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

class TaskMock {
  public:
    MOCK_METHOD(void, task, ());
};

class TestThreadPool : public testing::Test {
  public:
    virtual void SetUp() {
        pool = std::make_unique<thread_pool>(2);
        task_mock = new TaskMock();
        for (size_t i = 0; i < tasks_size; i++) {
            tasks.emplace_back([this] {
                task_mock->task();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    virtual void TearDown() { delete task_mock; }

    const size_t tasks_size = 20;
    std::unique_ptr<thread_pool> pool;
    TaskMock *task_mock;
    std::vector<std::function<void()>> tasks;
};

TEST_F(TestThreadPool, GivenThreadPoolIsCreatedWhenAddingATaskThenTheTaskIsExecuted) {
    EXPECT_CALL(*task_mock, task()).Times(1);
    pool->enqueue(tasks[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

TEST_F(TestThreadPool, GivenThreadPoolIsCreatedWhenAddingSeveralTaskThenAllTaskAreExecuted) {
    EXPECT_CALL(*task_mock, task()).Times(tasks.size());
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        pool->enqueue(*it);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
