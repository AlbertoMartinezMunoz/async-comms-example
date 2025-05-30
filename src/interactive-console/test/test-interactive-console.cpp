#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <interactive-console/interactive-console.hpp>


class TestInteractiveConsole : public ::testing::Test
{
public:

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(TestInteractiveConsole, WhenIfThenOK)
{
    ASSERT_EQ(0,0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
