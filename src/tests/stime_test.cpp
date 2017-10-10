#include <gtest/gtest.h>
#include "test_utils.hpp"
#include "../SnapTime.hpp"

/* Test STime functionality */

TEST (STimeTest, TestSTime) {
	CreateTestDirectory();
	// create an unsorted time
	TStrV id_vec;
	id_vec.Add("hello");
	id_vec.Add("world");
	TVec<TRawData> raw_data_vec;
	raw_data_vec.Add({1, "f"});
	raw_data_vec.Add({2, "t"});
	raw_data_vec.Add({0, "f"});
	TUnsortedTime unsorted(id_vec, raw_data_vec);

	TPt<TSTime> t = TSTime::TypedTimeGenerator(BOOLEAN, id_vec);
	t->AddUnsortedTime(unsorted);
	t->Sort();

	TStr fnm = TStr(ScratchDirectory) + "output.bin";
	TFOut fout(fnm);
	t->Save(fout);
	fout.Flush();

	TFIn fin(fnm);
	TPt<TSTime> tpt = TSTime::LoadSTime(fin);
	EXPECT_EQ(tpt->Len(), 3);
	EXPECT_EQ(*(TBool *)(tpt->DirectAccessValue(0)), TBool(false));
	EXPECT_EQ(*(TBool *)(tpt->DirectAccessValue(1)), TBool(false));
	EXPECT_EQ(*(TBool *)(tpt->DirectAccessValue(2)), TBool(true));

	for (int i=0; i<tpt->Len(); i++) {
		EXPECT_EQ(tpt->DirectAccessTime(i), (TTime) i);
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}