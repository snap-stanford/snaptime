void TSTimeParser::ReadRawData(TStr DirName) {
    std::cout << "Start Reading" << std::endl;
    DirCrawlMetaData dcmd (schema.IdNames.Len());
    ExploreDataDirs(DirName, dcmd, 0);
    FlushUnsortedData();
    std::cout << "Done Reading" << std::endl;
}

void TSTimeParser::ExploreDataDirs(TStr & DirName, DirCrawlMetaData dcmd, int DirIndex) {
    std::cout << "Explore Dirs "<< DirName.CStr() << std::endl;
    //adjust the metadata based on dir filename
    TStr DirBehavior = schema.Dirs[DirIndex];
    AdjustDcmd(DirName, DirBehavior, dcmd);

    //base case: at the end of the dirs, so this is an event file. Parse it
    if (DirIndex == schema.Dirs.Len()-1) {
        ReadEventDataFile(DirName, dcmd);
        return;
    }
    // otherwise, we're at a directory. Adjust the running id vec if necessary
    TStrV FnV;
    TTimeFFile::GetAllFiles(DirName, FnV, false); // get the directories
    for (int i=0; i< FnV.Len(); i++) {
        ExploreDataDirs(FnV[i], dcmd, DirIndex + 1);
    }
}

// read data file as according to schema
// dcmd passed in was a copy up above, so ok to modify
void TSTimeParser::ReadEventDataFile(TStr & FileName, DirCrawlMetaData & dcmd) {
    std::ifstream infile(FileName.CStr());
    AssertR(infile.is_open(), "could not open eventfile");
    std::cout << "reading file " << FileName.CStr() << std::endl;

    std::string line;
    while(std::getline(infile, line)) {
        TVec<TStr> row = TCSVParse::readCSVLine(line);
        if (CurrNumRecords % 1000 == 0) std::cout << "lines read " << CurrNumRecords << std::endl;
        AssertR(schema.FileSchema.Len() == row.Len(), "event file has incorrect number of columns");

        TVec<TPair<TStr, TStr>> sensor_vals; // <(SensorName, SensorValue), ...>
        // iterate through column values
        // std::cout<<"---"<<std::endl;
        for (int i=0; i<row.Len(); i++) {
            TPair<TStr, TColType> col = schema.FileSchema[i];
            TStr IDName = col.GetVal1();
            TColType ColBehavior = col.GetVal2();
            TStr DataVal = row[i];
            if (DataVal == TStr("")) continue; //don't deal with it if empty
            // std::cout << IDName.CStr() << std::endl;
            //deal with ID and time first
            switch(ColBehavior) {
                case NO_ID:
                    break;
                case TIME:
                    dcmd.ts = schema.ConvertTime(DataVal);
                    dcmd.TimeSet = true;
                    break;
                case ID:
                    //add DataVal as the ID value for IDName
                    AdjustDcmd(DataVal, IDName, dcmd);
                    break;
                case SENSOR:
                    TPair<TStr, TStr> sensor(IDName, DataVal);
                    sensor_vals.Add(sensor);
                    break;
            }
        }
        AssertR(dcmd.TimeSet, "invalid schema: time is never set");
        TStr sensorKey("SENSOR");
        //split by sensor
        for (int i=0; i<sensor_vals.Len(); i++) {
            TStr sensorName = sensor_vals[i].GetVal1();
            TStr sensorValue = sensor_vals[i].GetVal2();
            TTIdVec IDVector = dcmd.RunningIDVec;
            AssertR(schema.IDName_To_Index.IsKey(sensorKey), "invalid schema");
            TInt SensorIndex = schema.IDName_To_Index.GetDat(sensorKey);
            if (IDVector[SensorIndex] == TStr::GetNullStr()) {
                // only replace sensor in ID vec if not already there
                IDVector[SensorIndex] = sensorName;
            }
            AddDataValue(IDVector, sensorValue, dcmd.ts); //adds value and flushes if necessary
        }
    }
}

void TSTimeParser::AddDataValue(TTIdVec & IDVector, TStr & value, TTime ts) {
    TTRawData new_tv_pair = TTRawData(ts, value);
    if (!RawTimeData.IsKey(IDVector)) {
        TVec<TTRawData> new_time_data;
        new_time_data.Add(new_tv_pair);
        RawTimeData.AddDat(IDVector, new_time_data);
    } else {
        RawTimeData.GetDat(IDVector).Add(new_tv_pair);
    }
    CurrNumRecords++;
    if (CurrNumRecords >= MaxRecordCapacity) {
        std::cout << MaxRecordCapacity << std::endl;
        FlushUnsortedData();
        CurrNumRecords = 0;
    }
}


/*
 * DirBehavior can be:
 *  NULL: do nothing
 *  TIME: set dcmd's time
 *  DEFAULT: treat DirName as an ID under DirBehavior's IDName
 */
void TSTimeParser::AdjustDcmd(TStr & Name, TStr & Behavior, DirCrawlMetaData & dcmd) {
    if (Behavior == TStr("NULL")) return;
    if (Behavior == TStr("TIME")) {
        dcmd.ts = schema.ConvertTime(Name);
        dcmd.TimeSet = true;
    } else {
        AssertR(schema.IDName_To_Index.IsKey(Behavior), "Invalid schema");
        int IDIndex = schema.IDName_To_Index.GetDat(Behavior);
        // AssertR(dcmd.RunningIDVec[IDIndex] == TStr::GetNullStr(), "Invalid schema: repeat IDs");
        dcmd.RunningIDVec[IDIndex] = Name;
    }
}



//Create primary directory structure including indiv folder. return full directory path
TStr TSTimeParser::CreatePrimDirs(TTIdVec & IdVec) {
    //get the directory names based on hash
    TStrV dirNames;
    GetPrimDirNames(IdVec, dirNames);

    TStr dir = OutputDirectory;
    for (int i=0; i<dirNames.Len(); i++) {
        dir += TStr("/") + dirNames[i];
        if (!TDir::Exists(dir)) {
            AssertR(TDir::GenDir(dir), "Could not create directory");
        }
    }
    return dir;
}

// return a vector of the primary directory names for this idvec (including indiv folder)
void TSTimeParser::GetPrimDirNames(TTIdVec & IdVec, TStrV& result) {
    int primHash = IdVec.GetPrimHashCd();
    for (int i=0; i<ModHierarchy.Len(); i++) {
        int rem = primHash % ModHierarchy[i];
        result.Add(TInt(rem).GetStr());
    }
    result.Add(TSTimeParser::CreateIDVFileName(IdVec));
}

/// Should be ID, ID, ID... timestamp, value
// THis is old, TODO: Delete
void TSTimeParser::ReadEventFile(std::string filename) {
    std::ifstream infile(filename);
    AssertR(infile.is_open(), "could not open eventfile");
    std::string line;
    std::cout << "Num Records " << CurrNumRecords << std::endl;
    while(std::getline(infile, line)) {
        TTIdVec IDVector = TCSVParse::readCSVLine(line);
        if (CurrNumRecords % 1000 == 0) std::cout << "lines read " << CurrNumRecords << std::endl;
        AssertR(IDVector.Len() >= 2, "must have at least a TS and value");

        TStr value = IDVector.Last();
        IDVector.DelLast();

        TTime ts = IDVector.Last().GetUInt64();
        IDVector.DelLast();

        TTRawData new_tv_pair = TTRawData(ts, value);

        if (!RawTimeData.IsKey(IDVector)) {
            TVec<TTRawData> new_time_data;
            new_time_data.Add(new_tv_pair);
            RawTimeData.AddDat(IDVector, new_time_data);
        } else {
            RawTimeData.GetDat(IDVector).Add(new_tv_pair);
        }
        CurrNumRecords++;
        if (CurrNumRecords >= MaxRecordCapacity) {
            std::cout << MaxRecordCapacity << std::endl;
            FlushUnsortedData();
            CurrNumRecords = 0;
        }
    }
    FlushUnsortedData();
}

//fname is based on primary and secondary hash of ids
// primHash_secHash (does not include .bin)
TStr TSTimeParser::CreateIDVFileName(TTIdVec & IdVec) {
    TStr prim_hash = TInt::GetHexStr(IdVec.GetPrimHashCd()); //dirnames are based on hash of ids
    TStr sec_hash = TInt::GetHexStr(IdVec.GetSecHashCd()); //dirnames are based on hash of ids
    TStr result = prim_hash + TStr("_") + sec_hash;
    return result;
}

void TSTimeParser::FlushUnsortedData() {
    std::cout<< "Flushing data"<<std::endl;
    THash<TTIdVec, TVec<TTRawData> >::TIter it;
    time_t t = std::time(0);
    TUInt64 now = static_cast<uint64> (t);

    for (it = RawTimeData.BegI(); it != RawTimeData.EndI(); it++) {
        TTIdVec IdVec = it.GetKey();
        TVec<TTRawData> dat = it.GetDat();

        TUnsortedTime time_record(IdVec, dat);
        // create primary structure as necessary
        TStr dir_prefix = CreatePrimDirs(IdVec);
        TStr fn = dir_prefix + TStr('/') + now.GetStr() + TStr(".bin");
        TFOut outstream(fn);
        time_record.Save(outstream);
    }
    std::cout<< "done flushing" << std::endl;
}

void TSTimeParser::SortBucketedData(bool ClearData) {
    std::cout << OutputDirectory.CStr() << std::endl;
    TStrV FnV;
    int hierarchySize = ModHierarchy.Len() +1 ;// including the top level directory
    TraverseAndSortData(OutputDirectory, hierarchySize, ClearData);

}

// ----------
// STATIC
// ----------

void TSTimeParser::TraverseAndSortData(TStr Dir, int level, bool ClearData) {
    AssertR(level >= 0, "invalid level");
    if (level == 0) {
        SortBucketedDataDir(Dir, ClearData);
        std::cout<< Dir.CStr() << std::endl;
    } else {
        TStrV FnV;
        TTimeFFile::GetAllFiles(Dir, FnV, true); // get the directories
        for (int i=0; i<FnV.Len(); i++) {
            TraverseAndSortData(FnV[i], level - 1, ClearData);
        }
    }
}

// TODO: what if the vector is too big to hold in memory
void TSTimeParser::SortBucketedDataDir(TStr DirPath, bool ClearData) {
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
    TStr sensorName = IDs[schema.IDName_To_Index.GetDat(TStr("SENSOR"))];
    TType type = schema.defaultType;
    if (schema.SensorType.IsKey(sensorName)) {
        type = schema.SensorType.GetDat(sensorName);
    }

    RawDataCmp comparator;
    BucketedData.SortCmp(comparator);
    switch (type) {
        case BOOLEAN:
            TSTimeParser::WriteSortedData<TBool>(DirPath, IDs, BucketedData,
                [] (TStr s) {return TBool(s[0] == 'T' || s[0] == 't' || s[0] == '1');}, ClearData);
            break;
        case STRING:
            TSTimeParser::WriteSortedData<TStr>(DirPath, IDs, BucketedData,
                [] (TStr s) { return s;}, ClearData);
            break;
        case INTEGER:
            TSTimeParser::WriteSortedData<TInt>(DirPath, IDs, BucketedData,
                [] (TStr s) { return TInt(s.GetInt());}, ClearData);
            break;
        default:
            TSTimeParser::WriteSortedData<TFlt>(DirPath, IDs, BucketedData,
                [] (TStr s) { return TFlt(s.GetFlt());}, ClearData);
            break;
    }
}


