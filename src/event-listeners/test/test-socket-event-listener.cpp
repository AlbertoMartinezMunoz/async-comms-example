#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <event-listeners/socket-event-listener.hpp>

#include <memory>

class EventHandlerMock : public event_listener_handler {
    MOCK_METHOD(void, handle, ());
};

class TestSocketEventListener : public ::testing::Test {
  public:
    virtual void SetUp() {
        listener = std::make_unique<socket_event_listener>();
        event_handler = std::make_unique<EventHandlerMock>();
    }

    virtual void TearDown() {}

    std::unique_ptr<socket_event_listener> listener;
    std::unique_ptr<EventHandlerMock> event_handler;
};

TEST_F(TestSocketEventListener, WhenSendingIfOKThenMessageIsSent) { ASSERT_EQ(1, 0); }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
