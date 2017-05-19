#ifndef STIME_PARSER_H
#define STIME_PARSER_H

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

	omp_lock_t* file_sys_lock; // pointer to the file system lock
public:

	TSTimeParser() {
		MaxRecordCapacity = TUInt::Mx;
		CurrNumRecords = 0;
	}

	TSTimeParser(TStr OutputDir, TSchema* _Schema, TVec<int> _ModHierarchy,
		omp_lock_t* lock_p, TUInt MaxCapacity) : {

		file_sys_lock = lock_p;
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
	void ReadEventDataFile(const TStr & FileName, DirCrawlMetaData & dcmd);

private:

	// Reading data
	// void ExploreDataDirs(TStr & DirName, DirCrawlMetaData dcmd, int DirIndex);
	void ReadEventDataFile(const TStr & FileName, DirCrawlMetaData & dcmd);
	void AddDataValue(const TTIdVec & IDVector, TStr & value, TTime ts);

	// Saving Data
	void GetPrimDirNames(TTIdVec & IdVec, TStrV& result);
	TStr CreatePrimDirs(TTIdVec & IdVec);

};

#endif
