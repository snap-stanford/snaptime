#ifndef STIME_MANAGER_H
#define STIME_MANAGER_H
#include "stime_parser.hpp"

typedef TPair<TStr, TDirCrawlMetaData> TStrDCMD; /* represents info for one data file */

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSParserManager {
public:
	// EventFileQueue is a queue with information for bootstrapping a parser for each data file.
	// The queue is a vector of {path, dir crawl meta data}
	TVec<TPair<TStr, TDirCrawlMetaData> > EventFileQueue;
	// parsers managed by this stime manager
	TVec<TSTimeParser> parsers;
	// the number of threads used in the parser threadpool
	TInt NumThreads;
	// The schema for parsing the raw directory data
	TSchema Schema;
	// The output directory to place the primary parsed file hierarchy
	TStr OutputDirectory;
	// For each number in ModHierarchy, will create a directory level with the given number of subfolders
	TVec<int> ModHierarchy;
	// The max capacity for each parser
	TUInt MaxParserCapacity;
	// A lock to prevent concurrent access to adding files in the file system
	std::mutex filesys_lock;
public:
	TSParserManager(TStr _OutputDir, TStr SchemaFile, TVec<int> _MHierarchy,
		TInt _NumThreads = 1, TUInt _MaxParserCapacity = TUInt::Mx) : NumThreads(_NumThreads), 
		Schema(SchemaFile), OutputDirectory(_OutputDir), ModHierarchy(_MHierarchy),
		MaxParserCapacity(_MaxParserCapacity), filesys_lock() {
		// create the directory if it does not already exist
		if (!TDir::Exists(OutputDirectory)) TDir::GenDir(OutputDirectory);
		//create parsers
		for (int i=0; i<NumThreads; i++) {
			parsers.Add(TSTimeParser(_OutputDir, &Schema, _MHierarchy, &filesys_lock, MaxParserCapacity));
		}
	}

	// Read and parse raw data files from the given DirName, but do not sort them
	void ReadRawData(TStr DirName);
	// Sort the data created by ReadRawData
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

