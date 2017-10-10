#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "../SnapTime.hpp"

TEST (SchemaTest, TestSchema) {
	CreateTestDirectory();
	TVec<int> ModHierarchy;
	ModHierarchy.Add(2);
	ModHierarchy.Add(3);
	TSParserManager manager(ScratchDirPath, SchemaOnePath, ModHierarchy, 1, 20);
	manager.ReadRawData(InputData);
	manager.SortBucketedData(true);

	TStrV QuerySplit;
	QuerySplit.Add("Foo");
	QuerySplit.Add("Bar");
	TSTimeSymDir SymDirMaker(ScratchDirPath, ScratchSymbolicDir, QuerySplit, SchemaOnePath);
	SymDirMaker.CreateSymbolicDirs();

	TVec<FileQuery> Query;
	Query.Add({TStr("Foo"), TStr("AA")});
	TQueryResult r;
	SymDirMaker.QueryFileSys(Query, r, ScratchQueryOutput);
	std::cout<< r.Len() << std::endl;
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}