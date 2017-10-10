#include "SnapTime.hpp"
#include "stime.cpp"
#include "stime_helper.cpp"
#include "stime_parser.cpp"
#include "stime_schema.cpp"
#include "stime_manager.cpp"
#include "stime_sorter.cpp"
#include "stime_symdir.cpp"

void GeneratePrimaryDirectories(SnapTimeConfiguration config) {
	TVec<int> PrimaryHierarchy;
	for (int modFolder : config.ModHierarchy) {
		PrimaryHierarchy.Add(modFolder);
	}
	TSParserManager manager(TStr(config.PrimaryDirectory.c_str()), TStr(config.SchemaFile.c_str()),
		PrimaryHierarchy, TInt(config.NumThreads), 500000);
	manager.ReadRawData(TStr(config.InputRawDirectory.c_str()));
	manager.SortBucketedData(true);
}

void GenerateSymbolicIndex(SnapTimeConfiguration config) {
	TStrV QuerySplit;
	for (std::string split : config.SymbolicSplit) {
		QuerySplit.Add(TStr(split.c_str()));
	}
	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));
	SymDirMaker.CreateSymbolicDirs();
}

void QueryAndSaveSparse(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query, std::string OutputFile) {
	TQueryResult r;
	TStrV QuerySplit;
	for (std::string split : config.SymbolicSplit) {
		QuerySplit.Add(TStr(split.c_str()));
	}
	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));
	TVec<FileQuery> FileQueries;
	for (auto QueryValue : Query) {
		FileQueries.Add({TStr(QueryValue.first.c_str()), TStr(QueryValue.second.c_str())});
	}
	SymDirMaker.QueryFileSys(FileQueries, r, TStr(OutputFile.c_str()));
}

std::vector<std::vector<double> > QueryAndInflate(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query,
	std::string initialTimestamp, int duration, int granularity) {

	TQueryResult r;
	TStrV QuerySplit;
	for (std::string split : config.SymbolicSplit) {
		QuerySplit.Add(TStr(split.c_str()));
	}
	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));
	TVec<FileQuery> FileQueries;
	for (auto QueryValue : Query) {
		FileQueries.Add({TStr(QueryValue.first.c_str()), TStr(QueryValue.second.c_str())});
	}
	SymDirMaker.QueryFileSys(FileQueries, r, "");
	std::vector<std::vector<double> > result;
	SymDirMaker.InflateData(r, TStr(initialTimestamp.c_str()), duration, granularity, result);
	return result;
}

// std::vector<std::vector<double> > LoadQueryResultAndInflate(SnapTimeConfiguration config, std::string QueryResultFile,
// 	std::string initialTimestamp, int duration, int granularity) {
// 	TQueryResult r;
// 	TFIn instream(TStr(QueryResultFile.c_str()));
// 	r.Load(instream);

// 	TStrV QuerySplit;
// 	for (std::string split : config.SymbolicSplit) {
// 		QuerySplit.Add(TStr(split.c_str()));
// 	}
// 	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));
// 	std::vector<std::vector<double> > result;
// 	SymDirMaker.InflateData(r, TStr(initialTimestamp.c_str()), duration, granularity, result);
// 	return result;
// }