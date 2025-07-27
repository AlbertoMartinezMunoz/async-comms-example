#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands-implementations.hpp>

using ::testing::NiceMock;
using ::testing::Return;

class ShutdownReceiverMock : public shutdown_receiver {
  public:
    virtual ~ShutdownReceiverMock() {}
    MOCK_METHOD(int, shutdown, ());
};

template <class T> command *CreateShutdownCommand(shutdown_receiver *comms, shutdown_receiver *cli);

template <> command *CreateShutdownCommand<local_shutdown_command>(shutdown_receiver *comms, shutdown_receiver *cli) {
    return new local_shutdown_command(nullptr, comms, cli);
}

template <> command *CreateShutdownCommand<remote_shutdown_command>(shutdown_receiver *comms, shutdown_receiver *cli) {
    return new remote_shutdown_command(comms, cli);
}

template <typename T> class TestShutdownCommands : public testing::Test {
  public:
    virtual void SetUp() {
        comms_shutdown_mock = new ShutdownReceiverMock();
        cli_shutdown_mock = new ShutdownReceiverMock();
        shutdown_command = CreateShutdownCommand<T>(comms_shutdown_mock, cli_shutdown_mock);
    }

    virtual void TearDown() {
        delete shutdown_command;
        delete cli_shutdown_mock;
        delete comms_shutdown_mock;
    }

  protected:
    command *shutdown_command;
    ShutdownReceiverMock *comms_shutdown_mock;
    ShutdownReceiverMock *cli_shutdown_mock;
};

TYPED_TEST_SUITE_P(TestShutdownCommands);

TYPED_TEST_P(TestShutdownCommands, WhenExecutingShutdownCmdIfOKThenShutdownCommsAndCli) {
    EXPECT_CALL(*(this->cli_shutdown_mock), shutdown()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*(this->comms_shutdown_mock), shutdown()).Times(1).WillOnce(Return(0));
    this->shutdown_command->execute();
    ASSERT_EQ(0, 0);
}

REGISTER_TYPED_TEST_SUITE_P(TestShutdownCommands, WhenExecutingShutdownCmdIfOKThenShutdownCommsAndCli);

using ShutdownTestingTypes = ::testing::Types<local_shutdown_command, remote_shutdown_command>;
INSTANTIATE_TYPED_TEST_SUITE_P(TestCommands, TestShutdownCommands, ShutdownTestingTypes);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
