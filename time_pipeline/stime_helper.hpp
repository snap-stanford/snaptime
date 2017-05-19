#ifndef STIME_HELPER_H
#define STIME_HELPER_H

class TTimeFFile {
public:
  static void GetAllFiles(TStr& Path, TStrV& FnV, bool OnlyDirs = false);
};

class TCSVParse {
public:
	static void readCSVLine(std::string line, TVec<TStr> & result, char delim=',', bool TrimWs = true);
	static std::string trim(const std::string & str);
	static TStr CreateIDVFileName(const TTIdVec & IdVec);
};

#endif
