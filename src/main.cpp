#include "test.h"

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	test::test();
	return RUN_ALL_TESTS();
}
