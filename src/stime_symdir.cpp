/* for now these all get converted to floats. this should change eventually */
#include <limits.h>
#include <stdlib.h>
void TSTimeSymDir::InflateData(TQueryResult & r, TStr initialTs, int duration, int granularity, std::vector<std::vector<double> > & result) {
	TTime initialTimestamp = Schema.ConvertTime(initialTs);
	int indices[r.Len()];
	int size = duration/granularity;
	for (int i=0; i< r.Len(); i++) {
		std::vector<double> empty_row (size);
		result.push_back(empty_row);
		indices[0] = 0;
	}
	for (int i=0; i < size; i++) { // for each timestamp
		TTime ts = initialTimestamp + i*duration;
		for (int j=0; j<r.Len(); j++) { // for each result
 			TPt<TSTime> & data_ptr = r[j];
 			int new_index = AdvanceIndex(data_ptr, ts, indices[j]);
 			indices[j] = new_index;
 			result[j][i] = data_ptr->GetFloat(new_index);
 		}
 	}
}

int TSTimeSymDir::AdvanceIndex(TPt<TSTime> data_ptr, TTime time_stamp, int curr_index) {
	if (curr_index >= data_ptr->Len() - 1) {
		// at end of vector so keep index the same
		return curr_index;
	}
	int index_after;
	for (index_after = curr_index+1; index_after < data_ptr->Len(); index_after++) {
		TTime next_ts = data_ptr->DirectAccessTime(index_after);
		if (next_ts > time_stamp) {
			// we hit the end of the window, so break out
			return index_after -1;
		}
	}
	return index_after - 1;
}

void TSTimeSymDir::SaveQuerySet(TQueryResult & r, TSOut & SOut) {
	SOut.Save(r.Len());
	for (int i=0; i<r.Len(); i++) {
		r[i]->Save(SOut);
	}
}

void TSTimeSymDir::LoadQuerySet(TQueryResult & r, TSIn & SIn) {
	int length = 0;
	SIn.Load(length);
	for (int i=0; i<length; i++) {
		TPt<TSTime> queryRow = TSTime::LoadSTime(SIn);
		r.Add(queryRow);
	}
}

// Returns a query result. If OutputFile is not "", save into OutputFile
void TSTimeSymDir::QueryFileSys(TVec<FileQuery> Query, TQueryResult & r, TStr & InitialTimeStamp,
	TStr & FinalTimeStamp, TStr OutputFile) {
	// First find places where we can index by the symbolic filesystem
	THash<TStr, FileQuery> QueryMap;
	GetQuerySet(Query, QueryMap);
	TVec<FileQuery> ExtraQueries;
	// one query struct per directory split
	TVec<FileQuery> SymDirQueries(QuerySplit.Len()); // one filequery per querysplit
	for (int i=0; i<QuerySplit.Len(); i++) {
		TStr dir_name = QuerySplit[i];
		if (QueryMap.IsKey(dir_name)) {
			// query includes this directory name
			SymDirQueries[i] = QueryMap.GetDat(dir_name);
			QueryMap.DelKey(dir_name); // remove this query from the map
		} else {
			// this is empty, so we set it as an empty string
			SymDirQueries[i].QueryName = dir_name;
			SymDirQueries[i].QueryVal = TStr("");
		}
	}	
	// retrieve the data and put into an executable
	UnravelQuery(SymDirQueries, 0, OutputDir, QueryMap, r, InitialTimeStamp, FinalTimeStamp);
	if (OutputDir.Len() != 0) {
		TFOut outstream(OutputFile);
		SaveQuerySet(r, outstream);
	}
}

void TSTimeSymDir::UnravelQuery(TVec<FileQuery> & SymDirQueries, int SymDirQueryIndex, TStr& Dir,
	THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r, TStr & InitialTimeStamp, TStr & FinalTimeStamp) {

	if (SymDirQueryIndex == QuerySplit.Len()) {
		// base case: done traversing the symbolic directory, so we are in a directory
		// of pure event files. gather these event files into r
		GatherQueryResult(Dir, ExtraQueries, r, InitialTimeStamp, FinalTimeStamp);
		return;
	}
	if (SymDirQueries[SymDirQueryIndex].QueryVal != TStr("")) {
		// if this directory has a query value, go to that folder
		TStr path = Dir + TStr("/") + TTimeFFile::EscapeFileName(SymDirQueries[SymDirQueryIndex].QueryVal);
		AssertR(TDir::Exists(path), "Query does not exist in symbolic dir");
		UnravelQuery(SymDirQueries, SymDirQueryIndex+1, path, ExtraQueries, r, InitialTimeStamp, FinalTimeStamp);
	} else {
		// this directory doesn't have a query value, so queue up gathering in all subfolders
		TStrV FnV;
		TTimeFFile::GetAllFiles(Dir, FnV);
		for (int i=0; i<FnV.Len(); i++) {
			UnravelQuery(SymDirQueries, SymDirQueryIndex+1, FnV[i], ExtraQueries, r, InitialTimeStamp, FinalTimeStamp);
		}
	}
}

void TSTimeSymDir::GatherQueryResult(TStr FileDir, THash<TStr, FileQuery> & ExtraQueries, TQueryResult & r,
	TStr & InitialTimeStamp, TStr & FinalTimeStamp) {
	// Get bounding timestamps
	TTime initTS = 0;
	if (InitialTimeStamp.Len() != 0) {
		initTS = Schema.ConvertTime(InitialTimeStamp);
	}
	TTime finalTS = TTime::Mx;
	if (FinalTimeStamp.Len() != 0) {
		initTS = Schema.ConvertTime(FinalTimeStamp);
	}

	TStrV FnV;
	TTimeFFile::GetAllFiles(FileDir, FnV);
	for (int i=0; i<FnV.Len(); i++) {
		TStr FileName = FnV[i];
		TFIn inputstream(FileName);
		TPt<TSTime> t = TSTime::LoadSTime(inputstream, false);
		THash<TStr, FileQuery>::TIter it;
	    for (it = ExtraQueries.BegI(); it != ExtraQueries.EndI(); it++) {
	        TStr QueryName = it.GetKey();
	        TStr QueryVal = it.GetDat().QueryVal;
	        AssertR(Schema.KeyNamesToIndex.IsKey(QueryName), "Invalid query");
	        TInt IdIndex = Schema.KeyNamesToIndex.GetDat(QueryName);
	        if (t->KeyIds[IdIndex] != QueryVal) return; // does not match query
	    }
	    t->LoadData(inputstream);
	    t->TruncateVectorByTime(initTS, finalTS);
		r.Add(t);
	}
}

// fill in a hash from the query name to the actual query
void TSTimeSymDir::GetQuerySet(TVec<FileQuery> & Query, THash<TStr, FileQuery> & result) {
	for (int i=0; i<Query.Len(); i++) {
		result.AddDat(Query[i].QueryName, Query[i]);
	}
}

//--------
// Creating symbolic directory
void TSTimeSymDir::CreateSymbolicDirs() {
	if (FileSysCreated) return;
	TraverseEventFiles(InputDir);
	FileSysCreated = true;
}

void TSTimeSymDir::TraverseEventFiles(TStr& Dir) {
	if(!TDir::Exists(Dir)) {
		// this is the event file
		CreateSymDirsForEventFile(Dir);
	} else {
		TStrV FnV;
		TTimeFFile::GetAllFiles(Dir, FnV); // get the directories
		for (int i=0; i<FnV.Len(); i++) {
			TraverseEventFiles(FnV[i]);
		}
	}
}

void TSTimeSymDir::CreateSymDirsForEventFile(TStr & EventFileName) {
	TFIn inputstream(EventFileName);
	TPt<TSTime> t = TSTime::LoadSTime(inputstream, false);
	TStrV SymDirs;
	// find the dir names
	for (int i=0; i<QuerySplit.Len(); i++) {
		TStr & Query = QuerySplit[i];
		AssertR(Schema.KeyNamesToIndex.IsKey(Query), "Query to split on SymDir not found");
		TInt IDIndex = Schema.KeyNamesToIndex.GetDat(Query);
		SymDirs.Add(TTimeFFile::EscapeFileName(t->KeyIds[IDIndex]));
	}
	TStr path = OutputDir;
	for (int i=0; i<SymDirs.Len(); i++) {
		path = path + TStr("/") + SymDirs[i];
		if (!TDir::Exists(path)) {
			std::cout << "creating directory " << path.CStr() << std::endl;
			AssertR(TDir::GenDir(path), "Could not create directory");
		}
	}
	// create a sym link at the end of the path for this stime
	TStr final_path = path + TStr("/") + TCSVParse::CreateIDVFileName(t->KeyIds);
	char* real_event_path = realpath(EventFileName.CStr(), NULL);
	int success = symlink(real_event_path, final_path.CStr());
	free(real_event_path);
	AssertR(success != -1, "Failed to create symbolic directory");
}
