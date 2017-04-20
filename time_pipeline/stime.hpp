#ifndef TSTIME_H
#define TSTIME_H
#include "Snap.h"

typedef TUInt64 TTime; // time value
enum TType {BOOLEAN, STRING, INTEGER, FLOAT};

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
    TVec<TStr> KeyIds;
    TVec<TPair<TTime, TVal> > TimeData; // sorted by time
    TTime MinTime; // min time in range
    TTime MaxTime; // max time in range
};

/*
 * Intermediary framework unit. Identifiable by a list of IDs (who's ordering)
 * is defined by a global schema) and a TVec of time to value pairs,
 * but not sorted. Values are also just strings
 */
class TUnsortedTime {
public:
    TVec<TStr> KeyIds;
    TVec<TPair<TTime, TStr> > TimeData; // just a list of time values
public:
	TUnsortedTime() : KeyIds(), TimeData() {}
	TUnsortedTime(TVec<TStr> ids, TVec<TPair<TTime, TStr> > t_data) : KeyIds(ids), TimeData(t_data) {}
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