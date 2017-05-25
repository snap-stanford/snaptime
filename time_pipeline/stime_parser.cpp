void TDirCrawlMetaData::AdjustDcmd(const TStr & Name, const TStr & Behavior, TDirCrawlMetaData & dcmd, const TTSchema* schema) {
    if (Behavior == TStr("NULL")) return;
    if (Behavior == TStr("TIME")) {
        dcmd.ts = schema->ConvertTime(Name);
        dcmd.TimeSet = true;
    } else {
        AssertR(schema->IDName_To_Index.IsKey(Behavior), "Invalid schema");
        int IDIndex = schema->IDName_To_Index.GetDat(Behavior);
        dcmd.RunningIDVec[IDIndex] = Name;
    }
}


// read data file as according to schema
// dcmd passed in was a copy up above, so ok to modify
void TSTimeParser::ReadEventDataFile(TStr & FileName, TDirCrawlMetaData & dcmd) {
    std::ifstream infile(FileName.CStr());
    AssertR(infile.is_open(), "could not open eventfile");
    std::cout << "reading file " << FileName.CStr() << std::endl;

    int num_sensors = Schema->FileSchemaIndexList[SENSOR].Len();
    /* Create a cached Vector of the running sensors so that we don't keep hitting the hash
     * running_sensors:
     * {<values for sensor 1>, <values for sensor 2>, ... } */
    TVec<TStrV> running_sensors(num_sensors);
    /* Create a cached vector of all of the running times (one per row) corresponding
     * to the running_sensors */
    TVec<TTime> running_times;

    // keep track of the current ID vector. Empty if uninitialized
    TTIdVec running_id;
    TTime ts = dcmd.ts; //might not be set yet

    std::string line;
    while(std::getline(infile, line)) {
        TVec<TStr> row = TCSVParse::readCSVLine(line, Schema->FileDelimiter);
        if (CurrNumRecords % 1000 == 0) std::cout << "lines read " << CurrNumRecords << std::endl;
        AssertR(Schema->FileSchema.Len() == row.Len(), "event file has incorrect number of columns");

        // Adjust ID Vector in dcmd
        for (int i=0; i<Schema->FileSchemaIndexList[ID].Len(); i++) {
            TInt col_id = Schema->FileSchemaIndexList[ID][i]; // column index of ID
            TStr IDName = Schema->FileSchema[col_id].GetVal1();
            TStr val = row[col_id];
            // add the ID to the dcmd
            TDirCrawlMetaData::AdjustDcmd(val, IDName, dcmd, Schema);
        }

        //flush old values of running sensors
        if (running_times.Len() != 0 && running_id != dcmd.RunningIDVec) {
            FlushRunningVectors(running_sensors, running_times, running_id);
            running_id = dcmd.RunningIDVec;
            // TODO flush values
        } else if(running_id.Len() == 0) {
            // not initialized yet
            running_id = dcmd.RunningIDVec;
        }

        // Adjust TIME in dcmd
        if (Schema->FileSchemaIndexList[TIME].Len() > 0) {
            TInt col_id = Schema->FileSchemaIndexList[TIME][0];
            TStr val = row[col_id];
            ts = Schema->ConvertTime(val);
        }
        running_times.Add(ts);


        // Add sensor values to running
        for (int i=0; i<Schema->FileSchemaIndexList[SENSOR].Len(); i++) {
            TInt col_id = Schema->FileSchemaIndexList[SENSOR][i]; // column index of sensor
            TStr val = row[col_id];
            running_sensors[i].Add(val);
        }
    }
    // perform one last flush if necessary
    if (running_times.Len() != 0) {
        FlushRunningVectors(running_sensors, running_times, running_id);
    }
}


void TSTimeParser::FlushRunningVectors(TVec<TStrV> &running_sensors,
    TVec<TTime> & running_times, TTIdVec & running_id) {

    TStr sensorKey("SENSOR");
    TInt SensorIndex = Schema->IDName_To_Index.GetDat(sensorKey); // index into ID vector to put sensor val

    TTIdVec IdVector;

    for (int i=0; i< running_sensors.Len(); i++) {
        // for each sensor ..
        TStrV & sensor_data = running_sensors[i];
        AssertR(sensor_data.Len() == running_times.Len(), "current running sensor data not aligned with time vector");

        IdVector = running_id; // make a copy of the id vector
        TInt col_id = Schema->FileSchemaIndexList[SENSOR][i];
        TStr sensorName = Schema->FileSchema[col_id].GetVal1();
        if (IdVector[SensorIndex] == TStr::GetNullStr()) {
            IdVector[SensorIndex] = sensorName; // only replace sensor in ID vec if not already there
        }
        if (!RawTimeData.IsKey(IdVector)) {
            // add a vector for this key if it's not there already
            TVec<TTRawData> new_time_data;
            RawTimeData.AddDat(IdVector, new_time_data);
        }
        TVec<TTRawData> & stored_data_vec = RawTimeData.GetDat(IdVector);
        for (int j=0; j< sensor_data.Len(); j++) {
            // add data into vectors
            TTime ts = running_times[j];
            TStr value = sensor_data[j];
            TTRawData new_tv_pair = TTRawData(ts, value);
            stored_data_vec.Add(new_tv_pair);
            CurrNumRecords++;
        }
        if (CurrNumRecords >= MaxRecordCapacity) {
            std::cout << MaxRecordCapacity << std::endl;
            FlushUnsortedData();
            CurrNumRecords = 0;
        }
        sensor_data.Clr();
    }
    running_times.Clr();
    running_id.Clr();
}

void TSTimeParser::AddDataValue(const TTIdVec & IDVector, TStr & value, TTime ts) {
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
void TSTimeParser::GetPrimDirNames(const TTIdVec & IdVec, TStrV& result) {
    int primHash = IdVec.GetPrimHashCd();
    for (int i=0; i<ModHierarchy.Len(); i++) {
        int rem = primHash % ModHierarchy[i];
        result.Add(TInt(rem).GetStr());
    }
    result.Add(TCSVParse::CreateIDVFileName(IdVec));
}

void TSTimeParser::FlushUnsortedData() {
    std::cout<< "Flushing data"<<std::endl;
    // lock filesystem (no concurrent access)
    omp_set_lock(file_sys_lock);

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
    omp_unset_lock(file_sys_lock);
    RawTimeData.Clr();
    std::cout<< "done flushing" << std::endl;
}

