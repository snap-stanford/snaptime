#include "stime.hpp"

TPt<TSTime> TSTime::TypedTimeGenerator(TType type, TStrV& key_ids) {
	switch(type) {
		case BOOLEAN: {
			auto ptr = new TSTypedTime<TBool>(type, key_ids, [] (TStr & s) {return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1');});
			return TPt<TSTime> (ptr);
		} case STRING: {
			 auto ptr = new TSTypedTime<TStr>(type, key_ids,  [] (TStr & s) { return TStr(s.CloneCStr());});
			 return TPt<TSTime> (ptr);
		} case INTEGER: {
			 auto ptr = new TSTypedTime<TInt>(type, key_ids,  [] (TStr & s) { return TInt(s.GetUInt());});
			 return TPt<TSTime> (ptr);
		} default:{
			 auto ptr = new TSTypedTime<TFlt>(type, key_ids, [] (TStr & s) { return TFlt(s.GetFlt());});
			 return TPt<TSTime> (ptr);
		}
	}
}

TPt<TSTime> TSTime::LoadSTime(TFIn& FIn, bool ShouldLoadData) {
	int _type;
	FIn.Load(_type);
	TType type = static_cast<TType>(_type);
	TStrV key_ids(FIn);
	TPt<TSTime> t = TypedTimeGenerator(type, key_ids);
	if (ShouldLoadData) t->LoadData(FIn);
	return t;
}