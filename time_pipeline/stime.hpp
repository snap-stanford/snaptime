#ifndef TSTIME_H
#define TSTIME_H

#include "stime_protos.hpp"

/*
 * Basic time framework unit. Identifiable by a list of IDs (who's ordering)
 * is defined by a global schema) and a TVec of time to value pairs
 * For now defining time as a long long, but that might change
 * The IDs, for now, are strings
 * The timestamps are for some granularity (so metadata includes range)
 */
class TSTime {
public:
	TType stime_type; // will be saved as an int
    TStrV KeyIds;
    void* TimeDataPtr; // pointer to data (must free). Data must be a vector of (TTime, stime_type)
public:
	// toy constructor if only manipulating metadata
	TSTime() : KeyIds() {
		stime_type = INTEGER;
		TimeDataPtr = NULL;
	}

	TSTime(TSTime & t): stime_type(t.stime_type), KeyIds(t.KeyIds) {
		TimeDataPtr = t.TimeDataPtr;
	}

	TSTime(TType _type, TStrV _KeyIds) : stime_type(_type), KeyIds(_KeyIds) {
		CreateTimeData();
	}

	TSTime(TSIn& FIn) {
		TimeDataPtr = NULL;
		Load(FIn);
	}

	~TSTime() {
		FreeTimeData();
	}



	void FreeTimeData() {
		if (TimeDataPtr != NULL) {
			switch(stime_type) {
				case BOOLEAN: delete ((TVec<TPair<TTime, TBool> > *) TimeDataPtr); break;
                case STRING: delete ((TVec<TPair<TTime, TStr> > *) TimeDataPtr); break;
                case INTEGER: delete ((TVec<TPair<TTime, TInt> > *) TimeDataPtr); break;
                case FLOAT: delete ((TVec<TPair<TTime, TFlt> > *) TimeDataPtr); break;
			}
		}
		TimeDataPtr = NULL;
	}

	void Save(TSOut& FOut) {
		FOut.Save(static_cast<int>(stime_type));
		KeyIds.Save(FOut);
		if (TimeDataPtr != NULL) {
			switch(stime_type) {
				case BOOLEAN:((TVec<TPair<TTime, TBool> > *) TimeDataPtr)->Save(FOut); break;
				case STRING: ((TVec<TPair<TTime, TStr> > *) TimeDataPtr)->Save(FOut); break;
				case INTEGER: ((TVec<TPair<TTime, TInt> > *) TimeDataPtr)->Save(FOut); break;
				case FLOAT: ((TVec<TPair<TTime, TFlt> > *) TimeDataPtr)->Save(FOut); break;
			}
		}
	}

	// don't load the actual data
	void LoadMetaData(TSIn& FIn) {
		int _type;
		FIn.Load(_type);
		stime_type = static_cast<TType>(_type);
		KeyIds.Load(FIn);
	}	

	void Load(TSIn& FIn) {
		LoadMetaData(FIn);
		FreeTimeData();
		CreateTimeData();
		switch(stime_type) {
			case BOOLEAN: ((TVec<TPair<TTime, TBool> > *) TimeDataPtr)->Load(FIn); break;
			case STRING: ((TVec<TPair<TTime, TStr> > *) TimeDataPtr)->Load(FIn); break;
			case INTEGER: ((TVec<TPair<TTime, TInt> > *) TimeDataPtr)->Load(FIn); break;
			case FLOAT: ((TVec<TPair<TTime, TFlt> > *) TimeDataPtr)->Load(FIn); break;
		}
	}

private:
	void CreateTimeData() {
		switch(stime_type) {
			case BOOLEAN: TimeDataPtr = new TVec<TPair<TTime, TBool> > (); break;
			case STRING: TimeDataPtr = new TVec<TPair<TTime, TStr> > (); break;
			case INTEGER: TimeDataPtr = new TVec<TPair<TTime, TInt> > (); break;
			case FLOAT: TimeDataPtr = new TVec<TPair<TTime, TFlt> > (); break;
		}
		AssertR(TimeDataPtr != NULL, "allocation error");
	}
};

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
