#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H
#include "stime.hpp"
#include "stime_helper.hpp"

struct FileQuery
{
    TStr QueryName; //name of ID
    TStrV QueryVal; //empty vector if any value
};

/*
 * Reformatted FileQuery that uses schema indices
 * Formula is to sum QueryValIndex*Depth for all QueryIndexers for a result
 */
struct QueryIndexer
{
    TInt QueryNameIndex;             // Index of FileQuery.QueryName in KeyIDs
    THash<TStr, TInt> QueryValIndex; // {QueryVal -> Order Number}
    TStrV QueryVals;                 // QueryVals pulled from the Query Indexer
    TInt Depth;                      // Multiplier for this QueryIndexer.
                                     // This is equivalent to the size of a section
};

/*
 * QueryCollector is the datastructure in which we collect
 * Query results in a sorted manner. It's a 1D vector of
 * vectors of STime pointers, ID'd so that the vector
 * is in an appropriate order
 */

class QueryCollector
{
  private:
    TVec<TVec << TPt<TSTime>>> QueryGrid;
    TVec<QueryIndexer> QueryCompute;
    TSchema *schema;

  public:
    QueryCollector(TVec<FileQuery> Query, TSchema *_schema);

    void ConvertToTimeCollection(TTimeCollection &r, bool ZeroFlag);

    void AddSTimeToCollector(TPt<TSTime> elem);

  private:
    // Construct an empty TSTime with the correct IDs in the given index
    TPt<TSTime> ConstructEmptyTSTime(TInt index);
};

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSTimeSymDir
{
  public:
    TStr InputDir;
    TStr OutputDir;
    TStrV QuerySplit;
    TSchema Schema;
    TBool FileSysCreated;

  public:
    TSTimeSymDir(TStr _InputDir, TStr _OutputDir, TStrV _QuerySplit,
                 TStr SchemaFile) : InputDir(_InputDir), OutputDir(_OutputDir),
                                    QuerySplit(_QuerySplit), Schema(SchemaFile),
                                    FileSysCreated(false)
    {
        AssertR(TDir::Exists(InputDir), "Input directory must exist");
        if (!TDir::Exists(OutputDir))
            TDir::GenDir(OutputDir);
    }

    void CreateSymbolicDirs();
    static void LoadQuerySet(TTimeCollection &r, TSIn &SIn);

  public:
    void QueryFileSys(TVec<FileQuery> Query, TTimeCollection &r,
                      TStr &InitialTimeStamp, TStr &FinalTimeStamp, TStr OutputDir);
    void InflateData(TTimeCollection &r, TStr initialTimestamp, double duration,
                     double granularity, std::vector<std::vector<double>> &result);

  private:
    void TraverseEventFiles(TStr &Dir);

    void CreateSymDirsForEventFile(TStr &EventFileName);

    void GatherQueryResult(TStr FileDir, THash<TStr, FileQuery> &ExtraQueries,
                           QueryCollector &qCollector, TStr &InitialTimeStamp,
                           TStr &FinalTimeStamp);

    void UnravelQuery(TVec<FileQuery> &SymDirQueries, int SymDirQueryIndex,
                      TStr &Dir, THash<TStr, FileQuery> &ExtraQueries,
                      QueryCollector &qCollector, TStr &InitialTimeStamp,
                      TStr &FinalTimeStamp);

    void GetQuerySet(TVec<FileQuery> &Query, THash<TStr, FileQuery> &result);

    void SaveQuerySet(TTimeCollection &r, TSOut &SOut);

    int AdvanceIndex(TPt<TSTime> data_ptr, TTime time_stamp, int curr_index);

    void CreateQueryIndexers(TVec<FileQuery> Query);
};

// for finding summary statistics
void GetEventFileList(TStr &Dir, TStrV &Files);
void SummaryStats(TStr &RawDir, TStr &SchemaFile, TStr &OutputFile);

#endif
