#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <local-command-manager/local-command-manager.hpp>

class TestLocalCommandManager : public  ::testing::Test
{
public:
    virtual void SetUp()
    {
        
    }

    virtual void TearDown()
    {
        
    }
};

TEST_F(TestLocalCommandManager, Test)
{
    ASSERT_EQ(1, 1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
