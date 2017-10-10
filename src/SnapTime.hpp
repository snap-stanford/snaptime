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
	std::string InputRawDirectory; // The raw input directory
	std::string PrimaryDirectory; // The directory where we place primary raw processed files
	std::string SymbolicDirectory; // The directory where we place the symbolic query overlay
	std::string SchemaFile; // The location of the schema file
	std::vector<int> ModHierarchy; // The number of folders in each level of the Primary Directory
	std::vector<std::string> SymbolicSplit; // The KeyNames over which we divide out the symbolic overlay
	int NumThreads; //number of threads used for parsing
};

// Walk through the raw input directory to create the primary directory structure
void GeneratePrimaryDirectories(SnapTimeConfiguration config);

// Create the symbolic overlay after generating the primary directories
void GenerateSymbolicIndex(SnapTimeConfiguration config);

// Query the symbolic overlay and save the query result in the output file
void QueryAndSaveSparse(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query, std::string OutputFile);

// Query the symbolic overlay and inflate into the 2d vector
// time is in seconds
std::vector<std::vector<double> > QueryAndInflate(SnapTimeConfiguration config, std::vector<std::pair<std::string, std::string> > Query,
	std::string initialTimestamp, int duration, int granularity);

// // Load a previously saved query and inflate it into the 2d vector
// std::vector<std::vector<double> > LoadQueryResultAndInflate(SnapTimeConfiguration config, std::string QueryResultFile,
// 	std::string initialTimestamp, int duration, int granularity);




#endif //SNAPTIME_H
