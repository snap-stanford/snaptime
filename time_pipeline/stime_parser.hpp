#ifndef STIME_PARSER_H
#define STIME_PARSER_H

/*
 * Class: TDirCrawlMetaData
 * Keeps ID and time information when iterating through the raw files and directories
 */
class TDirCrawlMetaData {
public:
	TTIdVec RunningIDVec;
	TTime ts;
	bool TimeSet;
public:
	TDirCrawlMetaData() {
		ts = 0;
		TimeSet = false;
	}
	TDirCrawlMetaData(int numIds) : RunningIDVec(numIds){
		ts = 0;
		TimeSet = false;
	}
	/* Static method to adjust a DCMD
	 * If Behavior is:
	 * 		NULL: do nothing
	 *		TIME: set dcmd's time to Name converted as timestamp
	 *		default: treat Name as ID under Behavior's IDName
	 */
	static void AdjustDcmd(const TStr & Name, const TStr & Behavior, TDirCrawlMetaData & dcmd, const TTSchema* schema);
};

class TSTimeParser {
public:
	// Leave as TStr to TStr for now
	THash<TTIdVec, TTRawDataV> RawTimeData;
	TVec<int> ModHierarchy; //hierarchy of moduluses on prim_hash for primary filesystem
private:
	TStr OutputDirectory; // OutputDirectory to start file structure.
	TUInt CurrNumRecords;
	TUInt MaxRecordCapacity; // records to read before flushing files
	TTSchema* Schema; // pointer to schema
	std::mutex* mtx;
public:

	TSTimeParser() {
		MaxRecordCapacity = TUInt::Mx;
		CurrNumRecords = 0;
	}

	TSTimeParser(TStr OutputDir, TTSchema* _Schema, TVec<int> _ModHierarchy,
		std::mutex* _mtx, TUInt MaxCapacity) {

		mtx = _mtx;
		Schema = _Schema;
		ModHierarchy = _ModHierarchy;
		MaxRecordCapacity = MaxCapacity;
		OutputDirectory = OutputDir;
		CurrNumRecords = 0;
		AssertR(TDir::Exists(OutputDirectory), "Output directory must exist");
	}

	//create initial primary hierarhcy
	void FlushUnsortedData();
	// crawl through OutputDirectory given a schema
	// void ReadRawData(TStr DirName);
	void ReadEventDataFile(TStr FileName, TDirCrawlMetaData dcmd);

private:

	// Reading data
	// void ExploreDataDirs(TStr & DirName, TDirCrawlMetaData dcmd, int DirIndex);
	void AddDataValue(const TTIdVec & IDVector, TStr & value, TTime ts);
	void GetRawTimeListsForIDs(TVec<int> & KeyIDs, TTIdVec & running_id);
	// Saving Data
	void GetPrimDirNames(const TTIdVec & IdVec, TStrV& result);
	TStr CreatePrimDirs(TTIdVec & IdVec);

};

#endif
