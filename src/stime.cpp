#include "stime.hpp"

TSTime TSTime::TypedTimeGenerator(TType type, TStrV& key_ids) {
	switch(type) {
		case BOOLEAN:
			return TSTypedTime<TBool>(type, key_ids, [] (TStr & s) {return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1');});
		case STRING:
			return TSTypedTime<TStr>(type, key_ids,  [] (TStr & s) { return TStr(s.CloneCStr());});
		case INTEGER:
			return TSTypedTime<TInt>(type, key_ids,  [] (TStr & s) { return TInt(s.GetUInt());});
		default:
			return TSTypedTime<TFlt>(type, key_ids, [] (TStr & s) { return TFlt(s.GetFlt());});
	}
}

TSTime TSTime::LoadSTime(TFIn& FIn, bool ShouldLoadData) {
	int _type;
	FIn.Load(_type);
	TType type = static_cast<TType>(_type);
	TStrV key_ids(FIn);
	TSTime t = TypedTimeGenerator(type, key_ids);
	if (ShouldLoadData) t.LoadData(FIn);
}