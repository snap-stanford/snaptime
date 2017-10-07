#ifndef SNAPTIME_H
#define SNAPTIME_H

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream> 
#include <vector>
#include <mutex>
#include "ctpl_stl.h"
#include "Snap.h"
#include "stime.hpp"
#include "stime_schema.hpp"
#include "stime_parser.hpp"
#include "stime_symdir.hpp"
#include "stime_manager.hpp"
#include "stime_helper.hpp"
#include "stime_sorter.hpp"

struct SnapTimeConfiguration {
	std::string InputRawDirectory;
	std::string PrimaryDirectory;
	std::string SymbolicDirectory;
	std::string SchemaFile;
	std::vector<int> ModHierarchy;
	std::vector<std::string> SymbolicSplit;
	int NumThreads; //for parsing
};

void GeneratePrimaryDirectories(SnapTimeConfiguration config);

void GenerateSymbolicIndex(SnapTimeConfiguration config);

void QueryAndSaveSparse(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query, std::string OutputFile);

std::vector<std::vector<double> > QueryAndInflate(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query,
	std::string initialTimestamp, int duration, int granularity);

std::vector<std::vector<double> > LoadQueryResultAndInflate(SnapTimeConfiguration config, std::string QueryResultFile,
	std::string initialTimestamp, int duration, int granularity);




#endif //SNAPTIME_H
