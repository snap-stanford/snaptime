#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H
#include "stime.hpp"

typedef TVec<TPt<TSTime> > TQueryResult;

struct FileQuery {
	TStr QueryName; //name of ID
	TStr QueryVal; //empty string if any value
};

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSTimeSymDir {
public:
	TStr InputDir;
	TStr OutputDir;
	TStrV QuerySplit;
	TSchema Schema;
	TBool FileSysCreated;
public:
	TSTimeSymDir(TStr _InputDir, TStr _OutputDir, TStrV _QuerySplit,
		TStr SchemaFile) : InputDir(_InputDir), OutputDir(_OutputDir), 
		QuerySplit(_QuerySplit), Schema(SchemaFile), FileSysCreated(false) {	
		AssertR(TDir::Exists(InputDir), "Input directory must exist");
		if (!TDir::Exists(OutputDir)) TDir::GenDir(OutputDir);
		
	}

	void CreateSymbolicDirs();
	static void LoadQuerySet(TQueryResult & r, TSIn & SIn);

public:
	void QueryFileSys(TVec<FileQuery> Query, TQueryResult & r, TStr & InitialTimeStamp, TStr & FinalTimeStamp, TStr OutputDir);
	void InflateData(TQueryResult & r, TStr initialTimestamp, int duration, int granularity, std::vector<std::vector<double> > & result);
private:
	void TraverseEventFiles(TStr& Dir);
	void CreateSymDirsForEventFile(TStr & EventFileName); // TODO, be able to do this for time split
	void GatherQueryResult(TStr FileDir, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r,
		TStr & InitialTimeStamp, TStr & FinalTimeStamp);
	void UnravelQuery(TVec<FileQuery> & SymDirQueries, int SymDirQueryIndex,
		TStr& Dir, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r, TStr & InitialTimeStamp, TStr & FinalTimeStamp);
	void GetQuerySet(TVec<FileQuery> & Query, THash<TStr, FileQuery> & result);

	void SaveQuerySet(TQueryResult & r, TSOut & SOut);
	int AdvanceIndex(TPt<TSTime> data_ptr, TTime time_stamp, int curr_index);
};



#endif

