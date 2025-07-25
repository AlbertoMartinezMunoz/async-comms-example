#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <commands/commands.hpp>

class TestCommands : public testing::Test {
public:
  virtual void SetUp() {}

  virtual void TearDown() {}

protected:
};

TEST_F(TestCommands, TestNextSendHandlerMessage) { ASSERT_EQ(0, 0); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
