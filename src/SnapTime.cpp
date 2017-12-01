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

TTimeCollection Query(SnapTimeConfiguration config, QueryObject & Query, std::string OutputFile) {
	TTimeCollection r;
	TStrV QuerySplit;
	for (std::string split : config.SymbolicSplit) {
		QuerySplit.Add(TStr(split.c_str()));
	}
	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));

	TVec<FileQuery> FileQueries;
	for (auto QueryValue : Query.Queries) {
		FileQueries.Add({TStr(QueryValue.first.c_str()), TStr(QueryValue.second.c_str())});
	}
	TStr initTS(Query.InitialTimestamp.c_str());
	TStr finTS(Query.FinalTimestamp.c_str());
	SymDirMaker.QueryFileSys(FileQueries, r, initTS,
		finTS,TStr(OutputFile.c_str()));
	return r;
}

TTimeCollection LoadQuery(std::string InputFile) {
	TTimeCollection r;
	TFIn fin(TStr(InputFile.c_str()));
	TSTimeSymDir::LoadQuerySet(r, fin);
	return r;
}

// Only for doubles
std::vector<std::vector<double> > InflateQuery(SnapTimeConfiguration config, TTimeCollection r,
	std::string initTS, double duration, double granularity) {
	
	std::vector<std::vector<double> >  result;
	TStrV QuerySplit;
	for (std::string split : config.SymbolicSplit) {
		QuerySplit.Add(TStr(split.c_str()));
	}
	TSTimeSymDir SymDirMaker(TStr(config.PrimaryDirectory.c_str()), TStr(config.SymbolicDirectory.c_str()), QuerySplit, TStr(config.SchemaFile.c_str()));
	SymDirMaker.InflateData(r, TStr(initTS.c_str()), duration, granularity, result);
	return result;
}

void PerformSummaryStats(SnapTimeConfiguration config, std::string output_file) {
	TStr RawDir = TStr(config.PrimaryDirectory.c_str());
	TStr SchemaFile = TStr(config.SchemaFile.c_str());
	TStr OutputFile = TStr(output_file.c_str());
	SummaryStats(RawDir, SchemaFile, OutputFile);
}
