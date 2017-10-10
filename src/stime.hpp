#ifndef TSTIME_H
#define TSTIME_H

#include "stime_protos.hpp"

/*
 * Intermediary framework unit. Identifiable by a list of IDs (who's ordering)
 * is defined by a global schema) and a TVec of time to value pairs,
 * but not sorted. Values are also just strings
 */
class TUnsortedTime {
public:
    TStrV KeyIds;
    TVec<TRawData> TimeData; // just a list of time values
public:
	TUnsortedTime() : KeyIds(), TimeData() {}
	TUnsortedTime(TStrV & ids): KeyIds(ids), TimeData() {}
	TUnsortedTime(TStrV ids, TVec<TRawData> t_data) : KeyIds(ids), TimeData(t_data) {}
	void Save(TSOut& SOut) {
		KeyIds.Save(SOut);
		TimeData.Save(SOut);
	}
	void Load(TSIn& SIn) {
		KeyIds.Load(SIn);
		TimeData.Load(SIn);
	}
};

class TSTime {
public:
	TType Type;
	TStrV KeyIds;
	TCRef CRef;
public:
	TSTime() {}
	TSTime(TType type, TStrV& key_ids) : Type(type), KeyIds(key_ids), CRef() {}
	virtual ~TSTime()=0;
	TSTime(const TSTime & t) = delete; // cannot copy

	// saves STime, prepends with the type.  Note that "Load" is static
	// Converts string to value
	virtual void AddUnsortedTime(TUnsortedTime & RawData) = 0;
	// sort the time data.
	virtual void Sort() = 0;
	// Generate an empty TSTime
	static TPt<TSTime> TypedTimeGenerator(TType type, TStrV& key_ids);
	// Load a TSTime
	static TPt<TSTime> LoadSTime(TFIn& FIn, bool ShouldLoadData=true);
	void Save(TSOut &SOut) {
		SOut.Save(static_cast<int>(Type));
		KeyIds.Save(SOut);
		SaveData(SOut);
	}
	// Assumes that Type and keyid has already been loaded
	virtual void LoadData(TSIn &SIn) = 0;
	virtual void SaveData(TSOut &SOut) = 0;

	// This is a hack
	virtual TTime DirectAccessTime(int i) = 0;
	virtual void* DirectAccessValue(int i) = 0;
	virtual TInt Len() = 0;
};

template<typename TVal>
class TSTypedTime : public TSTime {
public:
	TVec<TPair<TTime, TVal> > TimeData;
	std::function<TVal(TStr&)> ConvertString;

	TSTypedTime(TType type, TStrV & key_ids, std::function<TVal(TStr&)> ConvertString_) : TSTime(type, key_ids),
		TimeData(), ConvertString(ConvertString_) {}

	~TSTypedTime() {}

	void SaveData(TSOut & SOut) {
		TimeData.Save(SOut);
	}
	void LoadData(TSIn & SIn) {
		TimeData.Load(SIn);
	}

	void AddUnsortedTime(TUnsortedTime & RawData) {
		for (int i=0; i<RawData.TimeData.Len(); i++) {
			TRawData & raw_data_val = RawData.TimeData[i];
			TimeData.Add({raw_data_val.Val1, ConvertString(raw_data_val.Val2)});
		}
	}
	void Sort() {
		TimeData.Sort();
	}

	TTime DirectAccessTime(int i) {
		AssertR(i < TimeData.Len(), "i is out of bounds for this STime");
		return TimeData[i].Val1;
	}

	void* DirectAccessValue(int i) {
		AssertR(i < TimeData.Len(), "i is out of bounds for this STime");
		return &(TimeData[i].Val2);
	}

	TInt Len() {
		return TimeData.Len();
	}

};

#endif
