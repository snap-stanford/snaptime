
// TODO: what if the vector is too big to hold in memory
void TSTimeSorter::SortBucketedDataDir(TStr DirPath, bool ClearData, TTSchema* schema_p) {
    TStrV FnV;
    // retrieve filenames
    TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
    TUnsortedTime unsorted_record;
    TTRawDataV BucketedData;
    for (int i=0; i<FnV.Len(); i++) {
        TStr filename = FnV[i];
        TFIn infile(filename);
        unsorted_record.Load(infile);
        BucketedData.AddV(unsorted_record.TimeData);
    }
    TTIdVec IDs = unsorted_record.KeyIds;

    // get type
    TStr sensorName = IDs[schema_p->IDName_To_Index.GetDat(TStr("SENSOR"))];
    TType type = schema_p->defaultType;
    if (schema_p->SensorType.IsKey(sensorName)) {
        type = schema_p->SensorType.GetDat(sensorName);
    }

    RawDataCmp comparator;
    BucketedData.SortCmp(comparator);
    switch (type) {
        case BOOLEAN:
            TSTimeSorter::WriteSortedData<TBool>(type, DirPath, IDs, BucketedData,
                [] (TStr s) {return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1');}, ClearData);
            break;
        case STRING:
            TSTimeSorter::WriteSortedData<TStr>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return s;}, ClearData);
            break;
        case INTEGER:
            TSTimeSorter::WriteSortedData<TInt>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return TInt(s.GetInt());}, ClearData);
            break;
        default:
            TSTimeSorter::WriteSortedData<TFlt>(type, DirPath, IDs, BucketedData,
                [] (TStr s) { return TFlt(s.GetFlt());}, ClearData);
            break;
    }
}