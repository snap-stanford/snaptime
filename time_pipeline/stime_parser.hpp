#ifndef STIME_PARSER_H
#define STIME_PARSER_H

class TSTimeParser {
public:
	// Leave as TStr to TStr for now
	THash<TTIdVec, TTRawDataV> RawTimeData;
	TVec<int> ModHierarchy; //hierarchy of moduluses on prim_hash for primary filesystem
private:
	TStr Directory; // directory to start file structure.
	TUInt CurrNumRecords;
	TUInt MaxRecordCapacity; // records to read before flushing files
	TTSchema schema;
private:
	class RawDataCmp {
	private:
		TCmp<TTime> cmp;
	public:
		int operator() (const TTRawData& x, const TTRawData& y) const {
			return cmp(x.Val1, y.Val1);
		}
	};

	class DirCrawlMetaData {
	public:
		TTIdVec RunningIDVec;
		TTime ts;
		bool TimeSet;
	public:
		DirCrawlMetaData(int numIds) : RunningIDVec(numIds){
			ts = 0;
			TimeSet = false;
		}
	};
public:
	TSTimeParser(TStr Dir, TStr SchemaFile, TUInt MaxCapacity = TUInt::Mx) {
		schema.ReadSchemaFile(SchemaFile);
		MaxRecordCapacity = MaxCapacity;
		Directory = Dir;
		CurrNumRecords = 0;
		if (!TDir::Exists(Directory)) TDir::GenDir(Directory);
	}

	TSTimeParser(TStr Dir, TStr SchemaFile, TVec<int> _ModHierarchy, TUInt MaxCapacity = TUInt::Mx) {
		schema.ReadSchemaFile(SchemaFile);
		ModHierarchy = _ModHierarchy;
		MaxRecordCapacity = MaxCapacity;
		Directory = Dir;
		CurrNumRecords = 0;
		if (!TDir::Exists(Directory)) TDir::GenDir(Directory);
	}
	//create initial primary hierarhcy
	void ReadEventFile(std::string FileName); // old method, TODO delete
	// crawl through Directory given a schema
	void ReadRawData(TStr DirName);
	void FlushUnsortedData();
	void SortBucketedData(bool ClearData = true);

private:

	// Reading data
	void ExploreDataDirs(TStr & DirName, DirCrawlMetaData dcmd, int DirIndex);
	void ReadEventDataFile(TStr & FileName, DirCrawlMetaData & dcmd);
	void AddDataValue(TTIdVec & IDVector, TStr & value, TTime ts);
	void AdjustDcmd(TStr & Name, TStr & Behavior, DirCrawlMetaData & dcmd);

	// Saving Data
	void GetPrimDirNames(TTIdVec & IdVec, TStrV& result);
	TStr CreatePrimDirs(TTIdVec & IdVec);
	static TStr CreateIDVFileName(TTIdVec & IdVec);
	void SortBucketedDataDir(TStr DirPath, bool ClearData);
	void TraverseAndSortData(TStr Dir, int level, bool ClearData);

	template<class TVal>
	static void WriteSortedData(TStr DirPath, TTIdVec& IDs, TTRawDataV& SortedData, TVal (*val_convert)(TStr),
		bool ClearData) {
		std::cout<<ClearData<<std::endl;
		// convert all strings into actual data types
		TSTime<TVal> result(IDs);
		for (int i=0; i < SortedData.Len(); i++) {
			TTime ts = SortedData[i].GetVal1();
			TVal val = val_convert(SortedData[i].GetVal2());
			TPair<TTime, TVal> new_val(ts, val);
			result.TimeData.Add(new_val);
		}
		TStr OutFile = DirPath + TStr("/") + CreateIDVFileName(IDs) + TStr(".out");
		if (ClearData) {
			std::cout << "clearing directory: " << DirPath.CStr() << std::endl;
			TStrV FnV;
			TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
			for (int i=0; i<FnV.Len(); i++) {
				std::cout << FnV[i].CStr() << std::endl;
				TFile::Del(FnV[i]);
			}
		}

		//save file out
		TFOut outstream(OutFile);
		result.Save(outstream);
		//TODO, put granularity
	}

};

#endif
