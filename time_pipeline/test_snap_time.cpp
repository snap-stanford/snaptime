#include "SnapTime.hpp"


int main( int argc, char* argv[] ){
	if (argc != 3) {
		std::cout << "expect <directory> <event file>" << std::endl;
		exit(0);
	}
	TStr dirname = TStr(argv[1]);
	std::string event_file(argv[2]);
	TSTimeParser parser(dirname);
	TStrV fn;
	fn.Add("abc");
	fn.Add("def");
	parser.CreatePrimDirs(fn);
	// TTimeFFile::GetAllDirs(dirname);
	// parser.SortBucketedData(true);
	// parser.ReadEventFile(event_file);
	// parser.SortBucketedData(TStr("testdir/101676B3_4B304146"), INTEGER,true);
  return 0;
}

