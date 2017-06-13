#ifndef STIME_HELPER_H
#define STIME_HELPER_H

class TTimeFFile {
public:
  static void GetAllFiles(TStr& Path, TStrV& FnV, bool OnlyDirs = false);
  static TStr GetLocalFileName(TStr & DirName);
  static TStr EscapeFileName(TStr & FileName);
};

class TCSVParse {
public:
	static TVec<TStr> readCSVLine(std::string line, char delim=',', bool TrimWs = true);
	static std::string trim(const std::string & str);
	static TStr CreateIDVFileName(const TTIdVec & IdVec);
};

#endif
