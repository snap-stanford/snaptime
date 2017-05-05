#ifndef TSTIME_H
#define TSTIME_H

typedef TUInt64 TTime; // time value
enum TType {BOOLEAN, STRING, INTEGER, FLOAT};
typedef TVec<TStr> TTIdVec; // id vector
typedef TPair<TTime, TStr> TTRawData; //time raw data, left as string
typedef TVec<TTRawData> TTRawDataV; // vector of raw data

/*
 * Basic time framework unit. Identifiable by a list of IDs (who's ordering)
 * is defined by a global schema) and a TVec of time to value pairs
 * For now defining time as a long long, but that might change
 * The IDs, for now, are strings
 * The timestamps are for some granularity (so metadata includes range)
 */
template <class TVal>
class TSTime {
public:
    TTIdVec KeyIds;
    TVec<TPair<TTime, TVal> > TimeData; // sorted by time
public:
	TSTime(TTIdVec _KeyIds) : KeyIds(_KeyIds), TimeData() {}
	void Save(TFOut& FOut) {
		KeyIds.Save(FOut);
		TimeData.Save(FOut);
	}
	void Load(TFIn& FIn) {
		KeyIds.Load(FIn);
		TimeData.Load(FIn);
	}
};

/*
 * Intermediary framework unit. Identifiable by a list of IDs (who's ordering)
 * is defined by a global schema) and a TVec of time to value pairs,
 * but not sorted. Values are also just strings
 */
class TUnsortedTime {
public:
    TTIdVec KeyIds;
    TTRawDataV TimeData; // just a list of time values
public:
	TUnsortedTime() : KeyIds(), TimeData() {}
	TUnsortedTime(TTIdVec ids, TTRawDataV t_data) : KeyIds(ids), TimeData(t_data) {}
	void Save(TFOut& FOut) {
		KeyIds.Save(FOut);
		TimeData.Save(FOut);
	}
	void Load(TFIn& FIn) {
		KeyIds.Load(FIn);
		TimeData.Load(FIn);
	}
};
#endif
