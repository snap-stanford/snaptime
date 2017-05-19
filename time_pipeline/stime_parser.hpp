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
	TTSchema Schema; // pointer to schema

	omp_lock_t* file_sys_lock; // pointer to the file system lock
public:

	TSTimeParser() {
		MaxRecordCapacity = TUInt::Mx;
		CurrNumRecords = 0;
	}

	TSTimeParser(TStr OutputDir, TStr SchemaFile, TVec<int> _ModHierarchy,
		omp_lock_t* lock_p, TUInt MaxCapacity) {
		Schema.ReadSchemaFile(SchemaFile);
		file_sys_lock = lock_p;
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

	// Saving Data
	void GetPrimDirNames(const TTIdVec & IdVec, TStrV& result);
	TStr CreatePrimDirs(TTIdVec & IdVec);

};

#endif
