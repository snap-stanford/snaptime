#ifndef STIME_PARSER_H
#define STIME_PARSER_H

/*
 * Class: TDirCrawlMetaData
 * Keeps ID and time information when iterating through the raw files and directories
 */
class TDirCrawlMetaData {
public:
	TStrV RunningIDVec; // a running list of the IDs for this run
	TTime ts; // the time associated with this data point
	bool TimeSet; // is the time set in this run
public:
	TDirCrawlMetaData() {
		ts = 0;
		TimeSet = false;
	}
	TDirCrawlMetaData(int numIds) : RunningIDVec(numIds){
		ts = 0;
		TimeSet = false;
	}
	// Add either Time or ID to  the dcmd
	// If KeyType == NO_ID or TIME, then ignore index and just use value
	// Otherwise, Index signifies the index into the KeyNames vector where this key should be placed
	static void AdjustDcmd(const TStr & Value, TInt Index, TKeyType KeyType, TDirCrawlMetaData & dcmd, const TSchema* schema);
};

class TSTimeParser {
public:
	// Leave as TStr to TStr for now
	THash<TStrV, TUnsortedTime > RawTimeData;
	TVec<int> ModHierarchy; //hierarchy of moduluses on prim_hash for primary filesystem
private:
	TStr OutputDirectory; // OutputDirectory to start file structure.
	TUInt CurrNumRecords;
	TUInt MaxRecordCapacity; // records to read before flushing files
	TSchema* Schema; // pointer to schema
	std::mutex* mtx;
public:

	TSTimeParser() {
		MaxRecordCapacity = TUInt::Mx;
		CurrNumRecords = 0;
	}

	TSTimeParser(TStr OutputDir, TSchema* _Schema, TVec<int> _ModHierarchy,
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
	// Given a single data file, parse the data file and load it into the running
	// time series vectors
	void ReadEventDataFile(TStr FileName, TDirCrawlMetaData dcmd);

private:
	// Reading data
	void AddDataValue(const TStrV & IDVector, TStr & value, TTime ts);
	void GetRawTimeListsForIDs(TVec<int> & KeyIDs, TStrV & running_id);
	// Saving Data
	void GetPrimDirNames(const TStrV & IdVec, TStrV& result);
	TStr CreatePrimDirs(TStrV & IdVec);

};

#endif
