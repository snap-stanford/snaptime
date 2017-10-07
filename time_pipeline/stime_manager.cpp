#include "stime_manager.hpp"
// Read and parse raw data files from the given DirName
void TSParserManager::ReadRawData(TStr DirName) {
    EventFileQueue.Clr();
    CollectRawData(DirName);
    ctpl::thread_pool p(NumThreads);
    for (int i=0; i<EventFileQueue.Len(); i++) {
        TStr fn = EventFileQueue[i].GetVal1();
        TDirCrawlMetaData dcmd = EventFileQueue[i].GetVal2();
        // this->parsers[0].ReadEventDataFile(fn, dcmd);
        auto parser_method = [this, fn, dcmd] (int id) { this->parsers[id].ReadEventDataFile(fn, dcmd);};
        p.push(parser_method);
    }
    p.stop(true);
    // perform last flushes
    for (int i=0; i<NumThreads; i++) {
        parsers[i].FlushUnsortedData();
    }
}

void TSParserManager::CollectRawData(TStr DirName) {
    std::cout << "Start Collecting Files" << std::endl;
    TDirCrawlMetaData dcmd (Schema.KeyNames.Len());
    ExploreDataDirs(DirName, dcmd, 0);
    std::cout << "Done Collecting Files" << std::endl;
}

void TSParserManager::ExploreDataDirs(TStr & DirName, TDirCrawlMetaData dcmd, int DirIndex) {
    std::cout << "Explore Dirs "<< DirName.CStr() << std::endl;
    //adjust the metadata based on dir filename
    TPair<TKeyType, TInt> DirBehavior = Schema.Dirs[DirIndex];
    TStr LocalFileName = TTimeFFile::GetLocalFileName(DirName);
    TDirCrawlMetaData::AdjustDcmd(LocalFileName, DirBehavior.Val2, DirBehavior.Val1, dcmd, &Schema);

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

void TSParserManager::SortBucketedData(bool ClearData) {
    // collect dirs to sort
    TVec<TStr> dirPaths;
    int hierarchySize = ModHierarchy.Len() +1 ;// including the top level directory
    TraverseBucketedData(OutputDirectory, hierarchySize, ClearData, dirPaths);

    // TODO: make this actually work in parallel delegate sorting tasks
    for (int i=0; i<dirPaths.Len(); i++) {
        TSTimeSorter::SortBucketedDataDir(dirPaths[i], ClearData, &Schema);
    }
}

void TSParserManager::TraverseBucketedData(TStr Dir, int level, bool ClearData, TVec<TStr> & DirPaths) {
    AssertR(level >= 0, "invalid level");
    if (level == 0) {
        // at sorted data level
        DirPaths.Add(Dir);
    } else {
        TStrV FnV;
        TTimeFFile::GetAllFiles(Dir, FnV, true); // get the directories
        for (int i=0; i<FnV.Len(); i++) {
            TraverseBucketedData(FnV[i], level - 1, ClearData, DirPaths);
        }
    }
}
