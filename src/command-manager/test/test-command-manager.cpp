#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <command-manager/command-manager.hpp>

class TestCommandManager : public ::testing::Test
{
public:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(TestCommandManager, WhenConnectingIfOKThenOK)
{
    ASSERT_EQ(0, 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
