#include "stime_helper.hpp"

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
