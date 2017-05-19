#ifndef STIME_MANAGER_H
#define STIME_MANAGER_H

typedef TPair<TStr, TDirCrawlMetaData> TStrDCMD; /* represents info for one data file */

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSParserManager {
public:
	/* collecting raw data */
	TVec<TStrDCMD> EventFileQueue;

	TVec<TSTimeParser> parsers;
	TInt NumThreads;
	TTSchema Schema;
	TStr OutputDirectory;
	TVec<int> ModHierarchy;
	TUInt MaxParserCapacity;

	omp_lock_t filesys_lock;
public:
	TSParserManager(TStr _OutputDir, TStr SchemaFile, TVec<int> _MHierarchy,
		TInt _NumThreads = 1, TUInt _MaxParserCapacity = TUInt::Mx) : NumThreads(_NumThreads), 
		Schema(), OutputDirectory(_OutputDir), ModHierarchy(_MHierarchy),
		MaxParserCapacity(_MaxParserCapacity), filesys_lock() {

		omp_set_num_threads(NumThreads);

		// create the directory if it does not already exist
		if (!TDir::Exists(OutputDirectory)) TDir::GenDir(OutputDirectory);

		/* Read Schema File */
		Schema.ReadSchemaFile(SchemaFile);

		// create filesys lock
		omp_init_lock(&filesys_lock);
		//create parsers
		for (int i=0; i<NumThreads; i++) {
			parsers.Add(TSTimeParser(_OutputDir, SchemaFile, _MHierarchy, &filesys_lock, MaxParserCapacity));
		}
	}

	/* Find all event data files and fill them into EventFileQueue */
	void ReadRawData(TStr DirName);
	void SortBucketedData(bool ClearData=true);
private:
	/* Find all event data files and fill them into EventFileQueue */
	void CollectRawData(TStr DirName);

	/* iterate through directory tree trying to find event data files.
	 * If an event data file is found, put info into EventFileQueue
	 * Parameters:
	 * 		DirName: directory name to start searching
	 *		dcmd: The DirCrawlMetadata collected thus far
	 * 		DirIndex: the director level (as specified by Dirs in the schema)*/
	void ExploreDataDirs(TStr & DirName, TDirCrawlMetaData dcmd, int DirIndex);


	/* for sorting */
	void TraverseBucketedData(TStr Dir, int level, bool ClearData, TVec<TStr> & DirPaths);
};

#endif

