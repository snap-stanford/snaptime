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
void TSTimeParser::ReadEventDataFile(TStr FileName, TDirCrawlMetaData dcmd) {
    std::ifstream infile(FileName.CStr());
    AssertR(infile.is_open(), "could not open eventfile");
    std::cout << "reading file " << FileName.CStr() << std::endl;

    // keep track of the current ID vector. Empty if uninitialized
    TTIdVec running_id;
    TTime ts = dcmd.ts; //might not be set yet
    TVec<int> KeyIDs;

    int line_no = 0;
    std::string line;
    while(std::getline(infile, line)) {
	line_no++;
        TVec<TStr> row = TCSVParse::readCSVLine(line, Schema->FileDelimiter);
        if (CurrNumRecords % 10000 == 0) std::cout << "lines read " << CurrNumRecords << " by " << " in " << FileName.CStr() << std::endl;

        // Adjust ID Vector in dcmd
        for (int i=0; i<Schema->FileSchemaIndexList[ID].Len(); i++) {
            TInt col_id = Schema->FileSchemaIndexList[ID][i]; // column index of ID
            TStr IDName = Schema->FileSchema[col_id].GetVal1();
            TStr val = row[col_id];
            // add the ID to the dcmd
            TDirCrawlMetaData::AdjustDcmd(val, IDName, dcmd, Schema);
        }

	if (running_id != dcmd.RunningIDVec) {
		// the existing running vectors are not the right ones, so swap
		running_id = dcmd.RunningIDVec;
		KeyIDs.Clr();
		GetRawTimeListsForIDs(KeyIDs, running_id);
	}

        // Adjust TIME in dcmd
        if (Schema->FileSchemaIndexList[TIME].Len() > 0) {
            TInt col_id = Schema->FileSchemaIndexList[TIME][0];
            TStr val = row[col_id];
            ts = Schema->ConvertTime(val);
        }

        // Add sensor values to running
        for (int i=0; i<Schema->FileSchemaIndexList[SENSOR].Len(); i++) {
            TInt col_id = Schema->FileSchemaIndexList[SENSOR][i]; // column index of sensor
            TStr val = row[col_id];
	    if (val == TStr("")) continue;
	    TTRawData new_tv_pair = TTRawData(ts, val);
	    RawTimeData[KeyIDs[i]].Add(new_tv_pair);
	    CurrNumRecords++;
        }

	if (CurrNumRecords >= MaxRecordCapacity) {
            FlushUnsortedData();
	    running_id.Clr();
	    KeyIDs.Clr();
            CurrNumRecords = 0;
        }

    }
}


// returns a vector of pointers to the vector of raw data
void TSTimeParser::GetRawTimeListsForIDs(TVec<int> & KeyIDs, TTIdVec & running_id) {
	int num_sensors = Schema->FileSchemaIndexList[SENSOR].Len();
        TStr sensorKey("SENSOR");
        TInt SensorIndex = Schema->IDName_To_Index.GetDat(sensorKey);
	// only one pointer because sensor is already identified
	if (running_id[SensorIndex] != TStr::GetNullStr()) {
		if (!RawTimeData.IsKey(running_id)) {
                	TVec<TTRawData> new_time_data;
            		RawTimeData.AddDat(running_id, new_time_data);
            	}
		int key_id = RawTimeData.GetKeyId(running_id);
		KeyIDs.Add(key_id);
		return;
	 }
        TTIdVec IdVector;
        IdVector = running_id;
	for (int i=0; i < num_sensors; i++) {
		TInt col_id = Schema->FileSchemaIndexList[SENSOR][i];
		TStr sensorName = Schema->FileSchema[col_id].GetVal1();
		IdVector[SensorIndex] = sensorName;
                if (!RawTimeData.IsKey(IdVector)) {
                        TVec<TTRawData> new_time_data;
                        RawTimeData.AddDat(IdVector, new_time_data);
                }
		int key_id = RawTimeData.GetKeyId(IdVector);
                KeyIDs.Add(key_id);
	}

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
    std::cout<< "waiting to flush data"<<std::endl;
    mtx->lock();
    // lock filesystem (no concurrent access)
    std::cout<< "Flushing data"<<std::endl;
    THash<TTIdVec, TVec<TTRawData> >::TIter it;
    time_t t = std::time(0);
    TUInt64 now = static_cast<uint64> (t);

    for (it = RawTimeData.BegI(); it != RawTimeData.EndI(); it++) {
        TTIdVec IdVec = it.GetKey();
	for (int i=0; i< IdVec.Len(); i++) {
		std::cout << IdVec[i].CStr() << ",";
	}
	std::cout << std::endl;
        TVec<TTRawData> dat = it.GetDat();

        TUnsortedTime time_record(IdVec, dat);
        // create primary structure as necessary
        TStr dir_prefix = CreatePrimDirs(IdVec);
        TStr fn = dir_prefix + TStr('/') + now.GetStr() + TStr(".bin");
        TFOut outstream(fn);
        time_record.Save(outstream);
    }
    std::cout << "about to unlock" << std::endl;
    mtx->unlock();
    RawTimeData.Clr();
    std::cout<< "done flushing" << std::endl;
}

