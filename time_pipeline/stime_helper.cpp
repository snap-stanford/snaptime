#include "stime_helper.hpp"
#include <algorithm>
#include <boost/tokenizer.hpp>

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
    // escape \, fields separated by ",", fields can be quoted with "
    boost::escaped_list_separator<char> sep( '\\', delim, '"' ) ;
    typedef boost::tokenizer< boost::escaped_list_separator<char> > boost_tokenizer;
    boost_tokenizer tok( line, sep );
    TVec<TStr> vec_line;
    for(boost_tokenizer::iterator beg= tok.begin(); beg!=tok.end(); ++beg)
    {
      std::string val = *beg;
      if (TrimWs) {
        val = trim(val);
      }
      vec_line.Add(TStr(val.c_str()));
    }
    return vec_line;
}

std::string TCSVParse::trim(std::string const& str)
{
    if(str.empty())
        return str;
    std::size_t firstScan = str.find_first_not_of(' ');
    std::size_t first = (firstScan == std::string::npos) ? str.length() : firstScan;
    std::size_t last = str.find_last_not_of(' ');
    return str.substr(first, last-first+1);
}
