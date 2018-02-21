#include "stime.hpp"

TPt<TSTime> TSTime::TypedTimeGenerator(TType type, TStrV &key_ids)
{
	switch (type)
	{
	case BOOLEAN:
	{
		auto ptr = new TSTypedTime<TBool>(type, key_ids,
										  [](TStr &s) { return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1'); });
		return TPt<TSTime>(ptr);
	}
	case STRING:
	{
		auto ptr = new TSTypedTime<TStr>(type, key_ids,
										 [](TStr &s) { return TStr(s.CloneCStr()); });
		return TPt<TSTime>(ptr);
	}
	case INTEGER:
	{
		auto ptr = new TSTypedTime<TInt64>(type, key_ids,
										   [](TStr &s) { return s.GetInt64(); });
		return TPt<TSTime>(ptr);
	}
	default:
	{
		auto ptr = new TSTypedTime<TFlt>(type, key_ids,
										 [](TStr &s) { return s.GetFlt(); });
		return TPt<TSTime>(ptr);
	}
	}
}

TBool TSTime::GetBool(int i)
{
	void *ptr = DirectAccessValue(i);
	switch (Type)
	{
	case BOOLEAN:
	{
		return *(TBool *)ptr;
	}
	case FLOAT:
	{
		return TBool(*(TFlt *)ptr != TFlt(0));
	}
	case INTEGER:
	{
		return TBool(*(TInt64 *)ptr != TInt64(0));
	}
	default:
	{
		AssertR(false, "cannot convert string to boolean");
	} // cannot convert string
	}
	return false;
}

TFlt TSTime::GetFloat(int i)
{
	void *ptr = DirectAccessValue(i);
	switch (Type)
	{
	case BOOLEAN:
	{
		return TFlt(((TBool *)ptr)->Val);
	}
	case FLOAT:
	{
		return *(TFlt *)ptr;
	}
	case INTEGER:
	{
		return TFlt(((TInt64 *)ptr)->Val);
	}
	default:
	{
		AssertR(false, "cannot convert string to float");
	} // cannot convert string
	}
	return 0;
}

TInt64 TSTime::GetInt(int i)
{
	void *ptr = DirectAccessValue(i);
	switch (Type)
	{
	case BOOLEAN:
	{
		return TInt64(((TBool *)ptr)->Val);
	}
	case FLOAT:
	{
		double val = ((TFlt *)ptr)->Val;
		return TInt64(int64(floor(val + 0.5)));
	}
	case INTEGER:
	{
		return *(TInt64 *)ptr;
	}
	default:
	{
		AssertR(false, "cannot convert string to integer");
	} // cannot convert string
	}
	return 0;
}

TStr TSTime::GetStr(int i)
{
	void *ptr = DirectAccessValue(i);
	switch (Type)
	{
	case BOOLEAN:
	{
		return TBool::GetStr(*(TBool *)ptr);
	};
	case FLOAT:
	{
		return TFlt::GetStr(*(TFlt *)ptr);
	}
	case INTEGER:
	{
		return ((TInt64 *)ptr)->GetStr();
	}
	default:
	{
		return *(TStr *)ptr;
	}
	}
	return "";
}

TPt<TSTime> TSTime::LoadSTime(TSIn &SIn, bool ShouldLoadData)
{
	int _type;
	SIn.Load(_type);
	TType type = static_cast<TType>(_type);
	TStrV key_ids(SIn);
	TPt<TSTime> t = TypedTimeGenerator(type, key_ids);
	if (ShouldLoadData)
		t->LoadData(SIn);
	return t;
}
