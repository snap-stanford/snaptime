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

TBool TSTime::GetBool(int i) {
	void* ptr = DirectAccessValue(i);
	switch(Type) {
		case BOOLEAN: {return *(TBool*) ptr;}
		case FLOAT: {return TBool(*(TFlt*)ptr != TFlt(0));}
		case INTEGER: {return TBool(*(TInt*)ptr != TInt(0));}
		default:{ AssertR(false, "cannot convert string to boolean");} // cannot convert string
	}
	return false;
}

TFlt TSTime::GetFloat(int i) {
	void* ptr = DirectAccessValue(i);
	switch(Type) {
		case BOOLEAN: {return TFlt(((TBool*) ptr)->Val);}
		case FLOAT: {return *(TFlt*)ptr;}
		case INTEGER: {return TFlt(((TInt*)ptr)->Val);}
		default:{ AssertR(false, "cannot convert string to float");} // cannot convert string
	}
	return 0;
}

TInt TSTime::GetInt(int i) {
	void* ptr = DirectAccessValue(i);
	switch(Type) {
		case BOOLEAN: {return TInt(((TBool*) ptr)->Val);}
		case FLOAT: {return TInt(TFlt::Round(((TFlt*)ptr)->Val));}
		case INTEGER: {return *(TInt*)ptr;}
		default:{ AssertR(false, "cannot convert string to integer");} // cannot convert string
	}
	return 0;
}

TStr TSTime::GetStr(int i) {
	void* ptr = DirectAccessValue(i);
	switch(Type) {
		case BOOLEAN: {
			return TBool::GetStr(*(TBool*)ptr);};
		case FLOAT: {return ((TFlt*)ptr)->GetStr();}
		case INTEGER: {return ((TInt*)ptr)->GetStr();}
		default:{ return *(TStr*)ptr;}
	}
	return "";
}

TPt<TSTime> TSTime::LoadSTime(TSIn& SIn, bool ShouldLoadData) {
	int _type;
	SIn.Load(_type);
	TType type = static_cast<TType>(_type);
	TStrV key_ids(SIn);
	TPt<TSTime> t = TypedTimeGenerator(type, key_ids);
	if (ShouldLoadData) t->LoadData(SIn);
	return t;
}

