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
    TStr EmptyStr = TStr("");
    std::string line;
    while(std::getline(infile, line)) {
	if (CurrNumRecords % 100000 == 0) std::cout << omp_get_thread_num() << " reading " << CurrNumRecords << std::endl;
        TVec<std::string> row;
        char delim;
        delim= Schema.FileDelimiter;
        std::istringstream is(line);
        std::string temp;
        while(getline(is, temp, delim)) {
             std::string val = TCSVParse::trim(temp);
             row.Add(val);
        }

	TVec<TPair<TStr, TStr>> sensor_vals; // <(SensorName, SensorValue), ...>
        AssertR(Schema.FileSchema.Len() == row.Len(), "event file has incorrect number of columns");	
        // iterate through column values
        for (int i=0; i<row.Len(); i++) {
            TPair<TStr, TColType> col = Schema.FileSchema[i];
            //TStr IDName = col.GetVal1();
            TColType ColBehavior = col.GetVal2();
            TStr DataVal = TStr(row[i].c_str());
            if (row[i].length() > 0) {
            	//deal with ID and time first
           	 switch(ColBehavior) {
                	case NO_ID:
                   	 	break;
                	case TIME:
                    		dcmd.ts = Schema.ConvertTime(TStr(row[i].c_str()));
                    		dcmd.TimeSet = true;
                    		break;
                	case ID:
                    		//add DataVal as the ID value for IDName
                    		TDirCrawlMetaData::AdjustDcmd(DataVal, Schema.FileSchema[i].GetVal1(), dcmd, &Schema);
                    		break;
               		 case SENSOR:
                   		sensor_vals.Add(TPair<TStr, TStr>(Schema.FileSchema[i].GetVal1(), DataVal));
                   	 	break;
		}
            }
	}
        AssertR(dcmd.TimeSet, "invalid schema: time is never set");
        TStr sensorKey("SENSOR");
	//split by sensor
        for (int i=0; i<sensor_vals.Len(); i++) {
            TStr sensorName = sensor_vals[i].GetVal1();
            TStr sensorValue = sensor_vals[i].GetVal2();
            TTIdVec IDVector = dcmd.RunningIDVec;
            AssertR(Schema.IDName_To_Index.IsKey(sensorKey), "invalid schema");
            TInt SensorIndex = Schema.IDName_To_Index.GetDat(sensorKey);
            if (IDVector[SensorIndex] == TStr::GetNullStr()) {
                // only replace sensor in ID vec if not already there
                IDVector[SensorIndex] = sensorName;
            }
            AddDataValue(IDVector, sensorValue, dcmd.ts); //adds value and flushes if necessary
        }
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

