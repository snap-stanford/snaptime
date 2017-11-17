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

struct QueryObject {
	std::vector<std::pair<std::string, std::string> > Queries; // IDName, ValueName
	std::string InitialTimestamp; // in same format as the schema, "" to ignore
	std::string FinalTimestamp; // in same format as schema, "" to ignore
};

// Walk through the raw input directory to create the primary directory structure
void GeneratePrimaryDirectories(SnapTimeConfiguration config);

// Create the symbolic overlay after generating the primary directories
void GenerateSymbolicIndex(SnapTimeConfiguration config);

// Query an object using the above QueryObject specifications, optionally saving the query into an outputfile
TTimeCollection Query(SnapTimeConfiguration config, QueryObject & Query, std::string OutputFile); // "" if do not want to save

// Load a previously saved query
TTimeCollection LoadQuery(std::string InputFile);

// Inflate a query into a double vector of doubles
std::vector<std::vector<double> > InflateQuery(SnapTimeConfiguration config, TTimeCollection r,
	std::string initTS, int duration, int granularity);

// Perform Summary stats
void PerformSummaryStats(SnapTimeConfiguration config, std::string output_file);

#endif //SNAPTIME_H
