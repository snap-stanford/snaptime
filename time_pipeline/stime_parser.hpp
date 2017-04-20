#ifndef STIME_PARSER_H
#define STIME_PARSER_H
#include "Snap.h"
#include "stime.hpp"
#include <string>
#include <iostream>
#include <fstream>



class TSTimeParser {
public:
	// Leave as TStr to TStr for now
	THash<TTIdVec, TTRawDataV> RawTimeData;
private:
	TStr Directory; // directory to start file structure.
	TUInt CurrNumRecords;
	TUInt MaxRecordCapacity; // records to read before flushing files
private:
	class RawDataCmp {
	private:
		TCmp<TTime> cmp;
	public:
		int operator() (const TTRawData& x, const TTRawData& y) const {
			return cmp(x.Val1, y.Val1);
		}
	};
public:
	TSTimeParser(TStr Dir, TUInt MaxCapacity = TUInt::Mx) {
		Directory = Dir;
		CurrNumRecords = 0;
	}
	void ReadEventFile(std::string FileName);
	void FlushUnsortedData();
public:
	static void SortBucketedData(TStr DirPath, TType type);
private:
	static TVec<TStr> readCSVLine(std::string line, char delim=',');
	template<class TVal>
	static void WriteSortedData(TStr DirPath, TTIdVec& IDs, TTRawDataV& SortedData) {
		//TODO, write sorted data out. 
		//TODO, put granularity
	}

};

class TTSchema {
public:
	THash <TInt, TStr> IdIndexName; //Hash of ID index to ID name
	// TType getType(TVec<TStr>& IdVec); // TODO return type for given ID vector
protected:
	class TSchemaNode {
	public:
		virtual TBool IsTerminal() = 0;
	};
	class TInternalSchemaNode : TSchemaNode {
	public:
		TInt KeyIndex;
		THash<TStr, TSchemaNode*> KeyToNode;
	public:
		TBool IsTerminal () {return false;}
	};

	class TLeafSchemaNode : TSchemaNode {
	public:
		TType type;
		//todo put granularity
	public:
		TBool IsTerminal () {return false;}
	};
};

#endif
