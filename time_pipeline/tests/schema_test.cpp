#include <gtest/gtest.h>
#include <limits.h>
#include <stdlib.h>

#include "../SnapTime.hpp"

TEST (SchemaTest, TestSchema) {
	char* full_path = realpath("tests/test_schemas/schema_1.schema", NULL);
	TSchema schema((TStr(full_path)));
	free(full_path);
	EXPECT_EQ(schema.KeyNames.Len(), 3);
	EXPECT_EQ(schema.SensorNames.Len(), 2);
	EXPECT_EQ(schema.Dirs.Len(), 3);
	EXPECT_EQ(schema.Cols.Len(), 4);
	EXPECT_EQ(1,1);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}