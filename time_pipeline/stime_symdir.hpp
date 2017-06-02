#ifndef STIME_SYMDIR_H
#define STIME_SYMDIR_H

/*
 * Class: TSParserManager
 * Delegates duties of collecting and sorting raw data out to various threads
 */
class TSTimeSymDir {
public:
	TStr InputDir;
	TStr OutputDir;
	TStrV QuerySplit;
	TTSchema* Schema;
public:
	TSSymDir(TStr _InputDir, TStr _OutputDir, TStrV _QuerySplit,
		TTSchema* _Schema) : InputDir(_InputDir), OutputDir(_OutputDir), 
		QuerySplit(_QuerySplit) {

		Schema = _Schema;
		AssertR(TDir::Exists(Input Directory), "Input directory must exist");
		if (!TDir::Exists(OutputDirectory)) TDir::GenDir(OutputDirectory);
	}

	void CreateSymbolicDirs();
private:
	void TraverseEventFiles(TStr& Dir);
	void CreateSymDirsForEventFile(TStr & EventFileName); // TODO, be able to do this for time split

};

#endif

