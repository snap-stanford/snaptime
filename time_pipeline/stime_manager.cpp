void TDirCrawlMetaData::AdjustDcmd(const TStr & Name, const TStr & Behavior, DirCrawlMetaData & dcmd, const TSchema* schema) {
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

void TSParserManager::ReadRawData(TStr DirName) {
    EventFileQueue.Clr();
    CollectRawData(DirName);
    // parallelize this
    #pragma omp parallel for
    for (int i=0; i<EventFileQueue.Len(); i++) {
        int thread_num = omp_get_thread_num();
        /* read each event file */
        parsers[thread_num].ReadEventDataFile(EventFileQueue[i].GetVal1(), EventFileQueue[i].GetVal2());
    }
    // perform last flushes
    for (int i=0; i<NumThreads; i++) {
        parsers[i].FlushUnsortedData();
    }
}

void TSParserManager::CollectRawData(TStr DirName) {
    std::cout << "Start Collecting Files" << std::endl;
    DirCrawlMetaData dcmd (Schema.IdNames.Len());
    ExploreDataDirs(DirName, dcmd, 0);
    std::cout << "Done Collecting Files" << std::endl;
}

void TSParserManager::ExploreDataDirs(TStr & DirName, DirCrawlMetaData dcmd, int DirIndex) {
    std::cout << "Explore Dirs "<< DirName.CStr() << std::endl;
    //adjust the metadata based on dir filename
    TStr DirBehavior = Schema.Dirs[DirIndex];
    TDirCrawlMetaData::AdjustDcmd(DirName, DirBehavior, dcmd, &schema);

    //base case: at the end of the dirs, so this is an event file. Add it to the
    // vector to be read later
    if (DirIndex == Schema.Dirs.Len()-1) {
        EventFileQueue.Add(TStrDCMD(DirName, dcmd));
        return;
    }
    // otherwise, we're at a directory.
    TStrV FnV;
    TTimeFFile::GetAllFiles(DirName, FnV, false); // get the directories
    for (int i=0; i< FnV.Len(); i++) {
        ExploreDataDirs(FnV[i], dcmd, DirIndex + 1);
    }
}

void SortBucketedData(bool ClearData=true) {
    // collect dirs to sort
    TVec<TStr> dirPaths;
    int hierarchySize = ModHierarchy.Len() +1 ;// including the top level directory
    TraverseBucketedData(OutputDirectory, hierarchySize, ClearData, dirPaths);

    // delegate sorting tasks
    #pragma omp parallel for
    for (int i=0; i<dirPaths.Len(); i++) {
        TSTimeSorter::SortBucketedDataDir(dirPaths[i], ClearData, &Schema);
    }
}

void TraverseBucketedData(TStr Dir, int level, bool ClearData, TVec<TStr> & DirPaths) {
    AssertR(level >= 0, "invalid level");
    if (level == 0) {
        // at sorted data level
        DirPaths.Add(Dir);
    } else {
        TStrV FnV;
        TTimeFFile::GetAllFiles(Dir, FnV, true); // get the directories
        for (int i=0; i<FnV.Len(); i++) {
            TraverseAndSortData(FnV[i], level - 1, ClearData, DirPaths);
        }
    }
}
