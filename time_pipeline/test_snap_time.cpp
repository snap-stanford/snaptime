#include "SnapTime.hpp"


int main( int argc, char* argv[] ){

	if (argc != 5) {
		std::cout << "wrong num args" << std::endl;
		exit(0);
	}
	time_t t_start = time(0);
	std::cout << "Start: " << (long long) t_start << std::endl;
	TStr SchemaFile(argv[1]);
	TStr InputDir(argv[2]);
	TStr OutputDir(argv[3]);

	TStr NumThreads(argv[4]);


	TVec<int> ModHierarchy;
	ModHierarchy.Add(29);
	ModHierarchy.Add(13);

	TSParserManager manager(OutputDir, SchemaFile, ModHierarchy, NumThreads.GetInt(), 10000000);
	manager.ReadRawData(InputDir);
	time_t t_raw = time(0);
	std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << std::endl;
	
	manager.SortBucketedData(true);
	time_t t_sort = time(0);
std::cout << "Start: " << (long long) t_start << ". Raw: " << (long long) t_raw << ". Sort " << (long long) t_sort << std::endl;

	// TSTimeParser parser(OutputDir, SchemaFile, ModHierarchy, 1000000);
	// parser.ReadRawData(InputDir);
	// time_t t_raw = time(0);
	// parser.SortBucketedData(true);
	// time_t t_pass = time(0);

	// std::cout << "Start: " << (long long) t_start << ". Schema: " << (long long) t_schema;
	// std::cout << ". First Pass: " << (long long) t_raw << ". Finished: " << (long long) t_pass << std::endl;

	// // TStr dirname = TStr(argv[1]);
	// std::string event_file(argv[2]);
	// TVec<int> ModHierarchy;
	// ModHierarchy.Add(29);
	// ModHierarchy.Add(13);
	// TSTimeParser parser(dirname, ModHierarchy, 100000);

	// TTimeFFile::GetAllDirs(dirname);
	// parser.SortBucketedData(true);
	// parser.ReadEventFile(event_file);
	// parser.SortBucketedData(TStr("testdir/101676B3_4B304146"), INTEGER,true);
  return 0;
}

