// TODO: what if the vector is too big to hold in memory
void TSTimeSorter::SortBucketedDataDir(TStr DirPath, bool ClearData, TSchema* schema_p) {
    std::cout << "attempting to sort " << DirPath.CloneCStr() << std::endl;
    TStrV FnV;
    // retrieve filenames
    TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
    TUnsortedTime unsorted_record;
    TVec<TRawData> BucketedData;
    for (int i=0; i<FnV.Len(); i++) {
        TStr filename = FnV[i];
        TFIn infile(filename);
        unsorted_record.Load(infile);
        BucketedData.AddV(unsorted_record.TimeData);
    }
    TStrV & IDs = unsorted_record.KeyIds;
    TType type = schema_p->GetType(IDs);

    RawDataCmp comparator;
    BucketedData.SortCmp(comparator);
    switch (type) {
        case BOOLEAN:
            TSTimeSorter::WriteSortedData<TBool>(type, DirPath, IDs, BucketedData,
                [] (TStr s) {return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1');}, ClearData);
            break;
        case STRING:
            TSTimeSorter::WriteSortedData<TStr>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return TStr(s.CloneCStr());}, ClearData);
            break;
        case INTEGER:
            TSTimeSorter::WriteSortedData<TInt>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return TInt(s.GetUInt());}, ClearData);
            break;
        default:
            TSTimeSorter::WriteSortedData<TFlt>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return TFlt(s.GetFlt());}, ClearData);
            break;
    }
}
