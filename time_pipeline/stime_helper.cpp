#include "stime_helper.hpp"


TStr TTimeFFile::GetLocalFileName(TStr & DirName) {
    std::string dir_str = std::string(DirName.CStr());
    if (dir_str[dir_str.length() - 1] == '/') {
        dir_str.erase(dir_str.length() - 1);
    }
    dir_str = dir_str.substr( dir_str.find_last_of("/") + 1 );
    return TStr(dir_str.c_str());
}

TStr TTimeFFile::EscapeFileName(TStr & FileName) {
	// remove forwardslashes from filename
	std::string fname = std::string(FileName.CStr());
	for (int i=fname.length() -1; i>=0; i--) {
		if (fname[i] == '/') fname.erase(i);
	}
	return TStr(fname.c_str());
}

//includes directories. returns the full path
void TTimeFFile::GetAllFiles(TStr& Path, TStrV& FnV, bool OnlyDirs){
  DIR* dir = opendir(Path.CStr());
  AssertR(dir != NULL, "Directory could not be opened");
  struct dirent *dir_entity = readdir(dir);
  while (dir_entity != NULL) {
  	if (strcmp(dir_entity->d_name, "..") != 0 && strcmp(dir_entity->d_name, ".") != 0) {
  		TStr dirname = Path + TStr("/") + TStr(dir_entity->d_name);
  		if (!OnlyDirs || TDir::Exists(dirname)) FnV.Add(dirname);
  	}
    dir_entity = readdir(dir);
  }
  closedir(dir);
}

TVec<TStr> TCSVParse::readCSVLine(std::string line, char delim, bool TrimWs) {
    TVec<TStr> vec_line;
    std::istringstream is(line);
    std::string temp;
    while(getline(is, temp, delim)) {
      std::string val = temp;
      if(TrimWs) {
        val = trim(val);
      }
      vec_line.Add(TStr(val.c_str()));
    }
    return vec_line;
}

std::string TCSVParse::trim(std::string str)
{
    if(str.empty())
        return str;
    char white_spaces[] = {' ', '\n', '\r'};
    for (int i=0; i<3; i++) {
      std::size_t firstScan = str.find_first_not_of(white_spaces[i]);
      std::size_t first = (firstScan == std::string::npos) ? str.length() : firstScan;
      std::size_t last = str.find_last_not_of(white_spaces[i]);
      str = str.substr(first, last-first+1);
    }
    return str;
}

//fname is based on primary and secondary hash of ids
// primHash_secHash (does not include .bin)
TStr TCSVParse::CreateIDVFileName(const TStrV & IdVec) {
    TStr prim_hash = TInt::GetHexStr(IdVec.GetPrimHashCd()); //dirnames are based on hash of ids
    TStr sec_hash = TInt::GetHexStr(IdVec.GetSecHashCd()); //dirnames are based on hash of ids
    TStr result = prim_hash + TStr("_") + sec_hash;
    return result;
}
