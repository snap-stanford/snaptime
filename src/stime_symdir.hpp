#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H

typedef TVec<TSTime> TQueryResult;

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
public:
	void QueryFileSys(TVec<FileQuery> Query, TQueryResult & r, TStr OutputDir);
	void InflateData(TQueryResult & r, TStr initialTimestamp, int duration, int granularity, std::vector<std::vector<double> > & result);
private:
	void TraverseEventFiles(TStr& Dir);
	void CreateSymDirsForEventFile(TStr & EventFileName); // TODO, be able to do this for time split
	void GatherQueryResult(TStr FileName, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r);
	void UnravelQuery(TVec<FileQuery> & SymDirQueries, int SymDirQueryIndex,
		TStr& Dir, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r);
	void GetQuerySet(TVec<FileQuery> & Query, THash<TStr, FileQuery> & result);

	template<class TVal> 
	static int AdvanceIndex(TSTime & data, TTime time_stamp, int curr_index) {
		return -1;
		// TVec<TPair<TTime, TVal> > * data_ptr = (TVec<TPair<TTime, TVal> > *) data.TimeDataPtr;
		// if (curr_index >= data_ptr->Len() - 1) {
		// 	// we're at the end of the vector, so keep the index exactly the same
		// 	return curr_index;
		// }
		// int index_after;
		// for (index_after= curr_index + 1; index_after < data_ptr->Len(); index_after++) {
		// 	TTime next_ts = data_ptr->GetVal(index_after).GetVal1();
		// 	if (next_ts > time_stamp) {
		// 		// we hit the end of the window, so break out
		// 		return index_after - 1;
		// 	}
		// }
		// return index_after -1;
	}
	static double GetValFromResult(TSTime & data, int index);
};



#endif

