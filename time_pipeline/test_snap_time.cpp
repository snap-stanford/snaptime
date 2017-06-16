#include "SnapTime.hpp"

void generate_primary(TStr InputDir, TStr OutputDir, TStr SchemaFile, TInt NumThreads) {

	TVec<int> ModHierarchy;
	ModHierarchy.Add(29);
	ModHierarchy.Add(13);
	time_t t_start = time(0);
	std::cout<< NumThreads << std::endl;
	TSParserManager manager(OutputDir, SchemaFile, ModHierarchy, NumThreads, 200000);
	manager.ReadRawData(InputDir);
	time_t t_raw = time(0);
	std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << std::endl;
	manager.SortBucketedData(true);
	time_t t_sort = time(0);
	std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << ". Sort " << (long long) t_sort << std::endl;
}

void create_sym_dirs(TStr InputDir, TStr OutputDir, TStr SchemaFile) {
	// perform symdir creation
	TStrV QuerySplit;
	QuerySplit.Add("MachineID");
	QuerySplit.Add("SENSOR");
	TSTimeSymDir SymDirMaker(InputDir, OutputDir, QuerySplit, SchemaFile);
	SymDirMaker.CreateSymbolicDirs();
}

void query(TStr InputDir, TStr OutputDir, TStr SchemaFile, TStr OutputFile) {
	// perform query
	TStrV QuerySplit;
	QuerySplit.Add("MachineID");
	QuerySplit.Add("SENSOR");
	TSTimeSymDir SymDirMaker(InputDir, OutputDir, QuerySplit, SchemaFile);

	TVec<FileQuery> Query;
	FileQuery q1 = {"MachineID", "2029717966592933890.csv.cut", false};
	Query.Add(q1);

	SymDirMaker.QueryFileSys(Query, OutputFile);
}

int main( int argc, char* argv[] ){
	// 1st arg must be test number
	//		0: generate primary dirs
	//		1: create sym links on top of primary dirs
	//		2: query a symbolic dir
	if (argc < 2) {
		std::cout << "need to at least specify an action" << std::endl;
	}

	TStr Action(argv[1]);
	TInt action = Action.GetInt();
	if (action == 0) {
		// Generate Primary Dirs Args:
		//		action: 0
		//		SchemaFile
		//		InputDir
		//		OutputDir
		//		NumThreads
		if (argc != 6) {
			std::cout << "wrong num args for generating primary dirs" << std::endl;
			exit(0);
		}
		TStr SchemaFile(argv[2]);
		TStr InputDir(argv[3]);
		TStr OutputDir(argv[4]);
		TStr NumThreads(argv[5]);
		TInt num_threads = NumThreads.GetInt();
		generate_primary(InputDir, OutputDir, SchemaFile, num_threads);
	} else if (action == 1) {
		// Query Test Args:
		//		action : 1
		// 		SchemaFile
		//		InputDir
		//		OutputDir
		if (argc != 5) {
			std::cout << "wrong num args for generating primary dirs" << std::endl;
			exit(0);
		}
		TStr SchemaFile(argv[2]);
		TStr InputDir(argv[3]);
		TStr OutputDir(argv[4]);
		create_sym_dirs(InputDir, OutputDir, SchemaFile);
	} else if (action == 2) {
		// Query Test Args:
		//		action : 2
		// 		SchemaFile
		//		InputDir
		//		OutputDir
		//		OutputFile
		if (argc != 6) {
			std::cout << "wrong num args for generating primary dirs" << std::endl;
			exit(0);
		}
		TStr SchemaFile(argv[2]);
		TStr InputDir(argv[3]);
		TStr OutputDir(argv[4]);
		TStr OutputFile(argv[5]);
		query(InputDir, OutputDir, SchemaFile, OutputFile);
	}
  return 0;
}

