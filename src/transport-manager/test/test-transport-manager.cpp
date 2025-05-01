#include <gtest/gtest.h>
#include <transport-manager/transport-manager.hpp>
#include <fff.h>

// int socket(int domain, int type, int protocol){
// 	(void)domain;
// 	(void)type;
// 	(void)protocol;
// 	return -1;
// }
DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(int, socket, int, int, int);

class TestAbstractFactoryPattern: public ::testing::Test {
public:
	TransportManager *transport = NULL;

	virtual void SetUp() {
		RESET_FAKE(socket);
		transport = new(TransportManager);
	}

	virtual void TearDown() {
		delete transport;
	}
};

TEST_F(TestAbstractFactoryPattern, ConnectWithoutErrors) {
	socket_fake.return_val = -1;
	EXPECT_EQ(0, transport->Connect());
	EXPECT_EQ(1, socket_fake.call_count);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
