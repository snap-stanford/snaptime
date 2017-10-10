// TODO: what if the vector is too big to hold in memory
void TSTimeSorter::SortBucketedDataDir(TStr DirPath, bool ClearData, TSchema* schema_p) {
    std::cout << "attempting to sort " << DirPath.CloneCStr() << std::endl;
    TStrV FnV;
    // retrieve filenames
    TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
    if (FnV.Len() == 0) return;
    // get the KeyIds
    TFIn infile(FnV[0]);
    TUnsortedTime unsorted_record;
    unsorted_record.Load(infile);
    TStrV & IDs = unsorted_record.KeyIds;
    TType type = schema_p->GetType(IDs);
    TSTime stime = TSTime::TypedTimeGenerator(type, IDs);
    stime.AddUnsortedTime(unsorted_record);
    // add the rest of the unsorted times
    for (int i=1; i<FnV.Len(); i++) {
        TFIn instream(FnV[i]);
        unsorted_record.Load(infile);
        stime.AddUnsortedTime(unsorted_record);
    }
    stime.Sort();
    
    TStr OutFile = DirPath + TStr("/") + TCSVParse::CreateIDVFileName(IDs) + TStr(".out");
    // clear directories
    if (ClearData) {
        std::cout << "clearing directory: " << DirPath.CStr() << std::endl;
        TStrV FnV;
        TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
        for (int i=0; i<FnV.Len(); i++) {
            std::cout << FnV[i].CStr() << std::endl;
            TFile::Del(FnV[i]);
        }
    }
    TFOut outstream(OutFile);
    stime.Save(outstream);


}
