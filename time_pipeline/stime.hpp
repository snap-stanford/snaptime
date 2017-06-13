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
class TSTime {
public:
	TType stime_type; // will be saved as an int
    TTIdVec KeyIds;
    void* TimeDataPtr; // pointer to data (must free). Data must be a vector of (TTime, stime_type)
public:
	// toy constructor if only manipulating metadata
	TSTime() : KeyIds() {
		stime_type = INTEGER;
		TimeDataPtr = NULL;
	}

	TSTime(TType _type, TTIdVec _KeyIds) : stime_type(_type), KeyIds(_KeyIds) {
		CreateTimeData();
	}

	TSTime(TFIn& FIn) {
		LoadMetaData(FIn);
		CreateTimeData();
		switch(stime_type) {
			case BOOLEAN: ((TVec<TPair<TTime, TBool> > *) TimeDataPtr)->Load(FIn); break;
			case STRING: ((TVec<TPair<TTime, TStr> > *) TimeDataPtr)->Load(FIn); break;
			case INTEGER: ((TVec<TPair<TTime, TInt> > *) TimeDataPtr)->Load(FIn); break;
			case FLOAT: ((TVec<TPair<TTime, TFlt> > *) TimeDataPtr)->Load(FIn); break;
		}
	}

	~TSTime() {
		if (TimeDataPtr != NULL) delete TimeDataPtr;
	}

	// don't load the actual data
	void LoadMetaData(TFIn& FIn) {
		int _type;
		FIn.Load(_type);
		stime_type = static_cast<TType>(_type);
		KeyIds.Load(FIn);
	}	

	void Save(TFOut& FOut) {
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

private:
	void CreateTimeData() {
		switch(stime_type) {
			case BOOLEAN: TimeDataPtr = new TVec<TPair<TTime, TBool> > (); break;
			case STRING: TimeDataPtr = new TVec<TPair<TTime, TStr> > (); break;
			case INTEGER: TimeDataPtr = new TVec<TPair<TTime, TInt> > (); break;
			case FLOAT: TimeDataPtr = new TVec<TPair<TTime, TFlt> > (); break;
		}
		AssertR(stime_type != NULL, "allocation error");
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
