/*
 * Container Class for methods to perform sorting of bucketed data (phase II)
 */
class TSTimeSorter {
private:
	class RawDataCmp {
	private:
		TCmp<TTime> cmp;
	public:
		int operator() (const TRawData& x, const TRawData& y) const {
			return cmp(x.Val1, y.Val1);
		}
	};
public:
	static void SortBucketedDataDir(TStr DirPath, bool ClearData, TSchema* schema_p);

private:
	template<class TVal>
	static void WriteSortedData(TType Type, TStr DirPath, TStrV& IDs, TVec<TRawData>& SortedData, TVal (*val_convert)(TStr),
		bool ClearData) {
		// convert all strings into actual data types
		TSTime result(Type, IDs);
		for (int i=0; i < SortedData.Len(); i++) {
			TTime ts = SortedData[i].GetVal1();
			TPair<TTime, TVal> new_val;
			new_val.Val1 = ts;
			new_val.Val2 = val_convert(SortedData[i].GetVal2());
			TVec<TPair<TTime, TVal> > * data_ptr = (TVec<TPair<TTime, TVal> > *) result.TimeDataPtr;
			data_ptr->Add(new_val);
		}
		TStr OutFile = DirPath + TStr("/") + TCSVParse::CreateIDVFileName(IDs) + TStr(".out");
		if (ClearData) {
			std::cout << "clearing directory: " << DirPath.CStr() << std::endl;
			TStrV FnV;
			TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
			for (int i=0; i<FnV.Len(); i++) {
				std::cout << FnV[i].CStr() << std::endl;
				TFile::Del(FnV[i]);
			}
		}

		//save file out
		TFOut outstream(OutFile);
		result.Save(outstream);
		//TODO, put granularity
	}

};
