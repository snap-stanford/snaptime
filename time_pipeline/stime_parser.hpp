#ifndef STIME_PARSER_H
#define STIME_PARSER_H
#include "Snap.h"
#include "stime.hpp"
#include <string>
#include <iostream>
#include <fstream>

typedef TVec<TStr> TTIdVec; // id vector
typedef TPair<TTime, TStr> TTRawData; //time raw data, left as string

class TSTimeParser {
public:
	// Leave as TStr to TStr for now
	THash<TTIdVec, TVec<TTRawData> > RawTimeData;
private:
	TStr Directory; // directory to start file structure.
	TUInt CurrNumRecords;
	TUInt MaxRecordCapacity; // records to read before flushing files
	
public:
	TSTimeParser(TStr Dir, TUInt MaxCapacity = TUInt::Mx) {
		Directory = Dir;
		CurrNumRecords = 0;
	}
	void ReadEventFile(std::string FileName);
	void FlushUnsortedData();
private:
	TVec<TStr> readCSVLine(std::string line, char delim=',');

};

class TTSchema {
public:
	THash <TInt, TStr> IdIndexName; //Hash of ID index to ID name
	// TType getType(TVec<TStr>& IdVec); //return type for given ID vector
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
