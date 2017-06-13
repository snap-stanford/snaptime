#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H

typedef TVec<TUnsortedTime> TQueryResult;

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSTimeSymDir {
public:
	TStr InputDir;
	TStr OutputDir;
	TStrV QuerySplit;
	TTSchema* Schema;
	TBool FileSysCreated;
public:
	TSTimeSymDir(TStr _InputDir, TStr _OutputDir, TStrV _QuerySplit,
		TTSchema* _Schema) : InputDir(_InputDir), OutputDir(_OutputDir), 
		QuerySplit(_QuerySplit), FileSysCreated(false) {

		Schema = _Schema;
		AssertR(TDir::Exists(InputDir), "Input directory must exist");
		if (!TDir::Exists(OutputDir)) TDir::GenDir(OutputDir);
	}

	void CreateSymbolicDirs();
public:
	struct FileQuery {
		TStr QueryName; //name of ID
		TStr QueryVal; //empty string if any value
		TBool IncludeAsVal; //include identifier
	};
	void QueryFileSys(TVec<FileQuery> Query, TStr OutputFile);
private:
	void TraverseEventFiles(TStr& Dir);
	void CreateSymDirsForEventFile(TStr & EventFileName); // TODO, be able to do this for time split
	void GatherQueryResult(TStr FileName, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r);
	void UnravelQuery(FileQuery* SymDirQueries, int SymDirQueryIndex,
		TStr& Dir, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r);
	void GetQuerySet(TVec<FileQuery> & Query, THash<TStr, FileQuery> & result);
};



#endif

