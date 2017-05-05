#ifndef STIME_HELPER_H
#define STIME_HELPER_H

class TTimeFFile {
public:
  static void GetAllFiles(TStr& Path, TStrV& FnV, bool OnlyDirs = false);
};

#endif
