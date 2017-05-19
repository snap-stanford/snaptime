typedef TPair<TStr, TDirCrawlMetaData> TStrDCMD; /* represents info for one data file */

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
	DirCrawlMetaData(int numIds) : RunningIDVec(numIds){
		ts = 0;
		TimeSet = false;
	}
	/* Static method to adjust a DCMD
	 * If Behavior is:
	 * 		NULL: do nothing
	 *		TIME: set dcmd's time to Name converted as timestamp
	 *		default: treat Name as ID under Behavior's IDName
	 */
	static AdjustDcmd(const TStr & Name, const TStr & Behavior, DirCrawlMetaData & dcmd, const TSchema* schema);
};

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
	TSchema Schema;
	TStr OutputDirectory;
	TVec<int> ModHierarchy;
	TUInt MaxParserCapacity;

	omp_init_lock filesys_lock;
public:
	TSParserManager(TStr _OutputDir, TStr _SchemaFile, TVec<int> _MHierarchy,
		TInt _NumThreads = 1, TUInt _MaxParserCapacity = TUInt::Mx) : NumThreads(_NumThreads), 
		Schema(), OutputDirectory(_OutputDir), ModHierarchy(_MHierarchy),
		MaxParserCapacity(_MaxParserCapacity), filesys_lock() {

		omp_set_num_threads(NumThreads);

		// create the directory if it does not already exist
		if (!TDir::Exists(OutputDirectory)) TDir::GenDir(OutputDirectory);

		/* Read Schema File */
		schema.ReadSchemaFile(SchemaFile);

		// create filesys lock
		omp_init_lock(&filesys_lock);
		//create parsers
		for (int i=0; i<NumThreads; i++) {
			parsers.Add(TSTimeParser(_OutputDir, &schema, _MHierarchy, &filesys_lock, MaxParserCapacity));
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
	void ExploreDataDirs(TStr & DirName, DirCrawlMetaData dcmd, int DirIndex);


	/* for sorting */
	void CollectBucketedData(TVec<TStr> & DirPaths);
	void TraverseBucketedData(TVec<TStr> & DirPaths);

};
