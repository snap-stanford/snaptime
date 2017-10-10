#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "../SnapTime.hpp"

TEST (STimeTest, TestSTime) {
	CleanTestDirectory();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}