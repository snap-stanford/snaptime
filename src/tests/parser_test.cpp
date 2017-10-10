#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "../SnapTime.hpp"

TEST (SchemaTest, TestSchema) {
	TVec<int> ModHierarchy;
	ModHierarchy.Add(2);
	ModHierarchy.Add(3);
	TSParserManager manager("tests/ScratchOutput/", "tests/test_schemas/schema_1.schema", ModHierarchy, 1, 20);
	manager.ReadRawData("tests/test_data/");
	manager.SortBucketedData(true);

	TStrV QuerySplit;
	QuerySplit.Add("Foo");
	QuerySplit.Add("Bar");
	TSTimeSymDir SymDirMaker("tests/ScratchOutput/", "tests/SymDirOutput/", QuerySplit, "tests/test_schemas/schema_1.schema");
	SymDirMaker.CreateSymbolicDirs();

	TVec<FileQuery> Query;
	Query.Add({TStr("Foo"), TStr("AA")});
	TQueryResult r;
	SymDirMaker.QueryFileSys(Query, r, "tests/QueryOutput");
	std::cout<< r.Len() << std::endl;
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}