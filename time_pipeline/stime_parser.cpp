#include "stime_parser.hpp"
void TDirCrawlMetaData::AdjustDcmd(const TStr & Value, TInt Index, TKeyType KeyType, TDirCrawlMetaData & dcmd, const TSchema* schema) {
    switch (KeyType) {
        case NO_ID:
            return;
        case TIME:
            dcmd.ts = schema->ConvertTime(Value);
            dcmd.TimeSet = true;
            return;
        case SENSOR:
            dcmd.RunningIDVec[Index] = Value;
            return;
        case ID:
            dcmd.RunningIDVec[Index] = Value;
            return;
        default:
            AssertR(false, "Invalid enum type when crawling data file");
    }
}

// read data file as according to schema
// dcmd passed in was a copy up above, so ok to modify
void TSTimeParser::ReadEventDataFile(TStr FileName, TDirCrawlMetaData dcmd) {
    std::ifstream infile(FileName.CStr());
    AssertR(infile.is_open(), "could not open eventfile");
    std::cout << "reading file " << FileName.CStr() << std::endl;
    int line_no = 0;
    std::string line;
    TStrV DataValues;
    while(std::getline(infile, line)) {
        line_no++;
        TVec<TStr> row = TCSVParse::readCSVLine(line, Schema->FileDelimiter);
        if (CurrNumRecords % 100000 == 0) std::cout << "lines read " << CurrNumRecords << " by " << " in " << FileName.CStr() << std::endl;
        TVec<TPair<TStr, TInt> > SensorValues; // {Value, Sensor Index}
        // read all the data in this row
        for (int i=0; i<Schema->Cols.Len(); i++) {
            if (Schema->Cols[i].Val1 == SENSOR) { // if sensor add to SensorValues and move on
                SensorValues.Add({row[i], Schema->Cols[i].Val2});
                continue;
            }
            TDirCrawlMetaData::AdjustDcmd(row[i], Schema->Cols[i].Val2, Schema->Cols[i].Val1, dcmd, Schema);
        }
        // Update parser with this data
        for (int i=0; i<SensorValues.Len(); i++) {
            TDirCrawlMetaData::AdjustDcmd(Schema->SensorNames[SensorValues[i].Val2], Schema->KeyNames.Len()-1, SENSOR, dcmd, Schema);
            if (!RawTimeData.IsKey(dcmd.RunningIDVec)) {
                RawTimeData.AddDat(dcmd.RunningIDVec, TUnsortedTime(dcmd.RunningIDVec));
            }
            RawTimeData.GetDat(dcmd.RunningIDVec).TimeData.Add({dcmd.ts, SensorValues[i].Val1});
            CurrNumRecords++;
        }
    	if (CurrNumRecords >= MaxRecordCapacity) {
            FlushUnsortedData();
        }
    }
}


//Create primary directory structure including indiv folder. return full directory path
TStr TSTimeParser::CreatePrimDirs(TStrV & IdVec) {
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
void TSTimeParser::GetPrimDirNames(const TStrV & IdVec, TStrV& result) {
    int primHash = IdVec.GetPrimHashCd();
    for (int i=0; i<ModHierarchy.Len(); i++) {
        int rem = primHash % ModHierarchy[i];
        result.Add(TInt(rem).GetStr());
    }
    result.Add(TCSVParse::CreateIDVFileName(IdVec));
}

void TSTimeParser::FlushUnsortedData() {
    mtx->lock();
    // lock filesystem (no concurrent access)
    std::cout<< "Flushing data"<<std::endl;
    time_t t = std::time(0);
    TUInt64 now = static_cast<uint64> (t);
    THash<TStrV, TUnsortedTime >::TIter it;
    for (it = RawTimeData.BegI(); it != RawTimeData.EndI(); it++) {
        TUnsortedTime & time_record = it.GetDat();
        // create primary structure as necessary
        TStr dir_prefix = CreatePrimDirs(time_record.KeyIds);
        TStr fn = dir_prefix + TStr('/') + now.GetStr() + TStr(".bin");
        std::cout<<"flushing at " << fn.CStr() << std::endl;
        TFOut outstream(fn);
        time_record.Save(outstream);
    }
    CurrNumRecords = 0;
    std::cout << "about to unlock" << std::endl;
    mtx->unlock();
    RawTimeData.Clr();
    std::cout<< "done flushing" << std::endl;
}

