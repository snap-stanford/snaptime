#include "SnapTime.hpp"

void generate_primary(TStr InputDir, TStr OutputDir, TStr SchemaFile, TInt NumThreads) {

	TVec<int> ModHierarchy;
	ModHierarchy.Add(29);
	ModHierarchy.Add(13);
	time_t t_start = time(0);
	TSParserManager manager(OutputDir, SchemaFile, ModHierarchy, NumThreads, 10000000);
	manager.ReadRawData(InputDir);
	time_t t_raw = time(0);
	std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << std::endl;
	manager.SortBucketedData(true);
	time_t t_sort = time(0);
	std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << ". Sort " << (long long) t_sort << std::endl;
}

void query(TStr InputDir, TStr OutputDir, TStr SchemaFile) {
	TStrV QuerySplit;
	QuerySplit.Add("MachineID");
	QuerySplit.Add("SENSOR");
	TSTimeSymDir SymDirMaker(InputDir, OutputDir, QuerySplit, SchemaFile);
	SymDirMaker.CreateSymbolicDirs();
}

int main( int argc, char* argv[] ){
	// 1st arg must be test number
	//		0: generate primary dirs
	//		1: query an already created directory
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
		//		action : 0
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
		query(InputDir, OutputDir, SchemaFile);

	}
  return 0;
}

