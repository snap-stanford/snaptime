#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H
#include "stime.hpp"
#include "stime_helper.hpp"

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
	static void LoadQuerySet(TTimeCollection & r, TSIn & SIn);

public:
	void QueryFileSys(TVec<FileQuery> Query, TTimeCollection & r, TStr & InitialTimeStamp, TStr & FinalTimeStamp, TStr OutputDir);
	void InflateData(TTimeCollection & r, TStr initialTimestamp, int duration, int granularity, std::vector<std::vector<double> > & result);
private:
	void TraverseEventFiles(TStr& Dir);
	void CreateSymDirsForEventFile(TStr & EventFileName); // TODO, be able to do this for time split
	void GatherQueryResult(TStr FileDir, THash<TStr, FileQuery> & ExtraQueries, TTimeCollection & r,
		TStr & InitialTimeStamp, TStr & FinalTimeStamp);
	void UnravelQuery(TVec<FileQuery> & SymDirQueries, int SymDirQueryIndex,
		TStr& Dir, THash<TStr, FileQuery> & ExtraQueries, TTimeCollection & r, TStr & InitialTimeStamp, TStr & FinalTimeStamp);
	void GetQuerySet(TVec<FileQuery> & Query, THash<TStr, FileQuery> & result);

	void SaveQuerySet(TTimeCollection & r, TSOut & SOut);
	int AdvanceIndex(TPt<TSTime> data_ptr, TTime time_stamp, int curr_index);
};

// for finding summary statistics
namespace summary {

	void GetEventFileList(TStr & Dir, TStrV & Files) {
		if(!TDir::Exists(Dir)) {
			Files.Add(Dir);
		} else {
			TStrV FnV;
			TTimeFFile::GetAllFiles(Dir, FnV); // get the directories
			for (int i=0; i<FnV.Len(); i++) {
				GetEventFileList(FnV[i], Files);
			}
		}
	}
	void SummaryStats(TStr & RawDir, TStr & SchemaFile, TStr & OutputFile) {
		TSchema Schema(SchemaFile);
		TStrV EventFileList;
		GetEventFileList(RawDir, EventFileList);
		TVec<TStrV> rows;
		for (int i = 0; i<EventFileList.Len(); i++) {
			TFIn inputstream(EventFileList[i]);
			TPt<TSTime> t = TSTime::LoadSTime(inputstream, false);
			TStrV row = t->KeyIds;
			TInt length = t->Len();
			TTime t_zero = t->DirectAccessTime(0);
			TTime t_last = t->DirectAccessTime(length-1);
			TStr t_zero_str = Schema.ConvertTimeToStr(t_zero);
			TStr t_last_str = Schema.ConvertTimeToStr(t_last);
			row.Add(t_zero_str); // start time
			row.Add(t_last_str); // end time
			row.Add(length.GetStr()); // number of values
			rows.Add(row);
		}
		rows.Sort();
		TFOut outstream(OutputFile);

		for (int i = 0; i<rows.Len(); i++) {
			for (int j=0; j<rows[i].Len(); j++) {
				outstream.PutStr(rows[i][j]);
				outstream.PutCh(',');
			}
			outstream.PutLn();
		}
		// Go through each ID
	}
}



#endif

