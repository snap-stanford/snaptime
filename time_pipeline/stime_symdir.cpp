void TSTimeSymDir::CreateSymbolicDirs() {
	TraverseEventFiles(InputDir);
}

void TSTimeSymDir::TraverseEventFiles(TStr& Dir) {
	if(!TDir::Exists(Dir)) {
		// this is the event file
		CreateSymDirForEventFile(Dir);
	} else {
		TStrV FnV;
        TTimeFFile::GetAllFiles(Dir, FnV, true); // get the directories
        for (int i=0; i<FnV.Len(); i++) {
            TraverseEventFiles(FnV[i]);
        }
	}
}

void TSTimeSymDir::CreateSymDirsForEventFile(TStr & EventFileName) {
	TFIn inputstream(EventFileName);
	TStime t(FIn); // load in the data
	TStrV SymDirs;
	// find the dir names
	for (int i=0; i<QuerySplit.Len(); i++) {
		TStr & Query = QuerySplit[i];
		AssertR(Schema->IDName_To_Index.IsKey(Query), "Query to split on SymDir not found");
		TInt IDIndex = Schema->IDName_To_Index.GetDat(Query);
		SymDirs.Add(t.KeyIds[IDIndex]);
	}
	// for each directory level, create the dir if it doesn't exist
	TStr path = OutputDir;
	for (int i=0; i<SymDirs.Len(); i++) {
		path = path + TStr("/") + SymDir[i];
		if (!TDir::Exists(dir)) {
			std::cout << "creating directory " << path.CStr() << std::endl;
            AssertR(TDir::GenDir(path), "Could not create directory");
        }
	}
	// create a sym link at the end of the path for this stime
	final_path = path + TStr("/") + TCSVParse::CreateIDVFileName(t.KeyIds);
	int success = symlink(EventFileName, final_path);
	AssertR(success != -1, "Failed to create symbolic directory");
}