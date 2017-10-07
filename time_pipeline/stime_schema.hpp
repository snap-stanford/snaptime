#ifndef STIME_SCHEMA_H
#define STIME_SCHEMA_H

#include "stime_protos.hpp"

#define NUM_SCHEMA_SECTIONS 4

// All Key Names will be distilled down into indices in their key vector
// So we need a mapping from key name to column index, but for all other data
// structures we just refer by column index
// 
class TSchema {

public:
  enum SchemaCategory{FILE_HIERARCHY, SCHEMA, TYPES, TIME_CONVERT};

  // A vector of key names in their appropriate indices in a key vector
  // "SENSOR" should be the last name in the list
  TStrV KeyNames;

  // A vector of sensor names in the order they appear in a data file
  TStrV SensorNames;

  // a map of key names to their corresponding index in an key vector
  // SENSOR will be keyed under "SENSOR"
  THash<TStr, TInt> KeyNamesToIndex;

  // Vector specifying directory behavior. Element is {behavior, index}. 
  // Index should be -1 for NO_ID or TIME
  TVec<TPair<TKeyType, TInt> > Dirs;

  // Vector specifying column behavior. Element is {behavior, index}
  // for behavior = ID, index is the key name's index into the key vector.
  // for behavior = SENSOR, the index is the sensor name's index into the
  // SensorNames vector.
  TVec<TPair<TKeyType, TInt> > Cols;

  // Vector of Hashes from ID/sensor name to type. If not in the hash, assume default.
  // Each Hash corresponds to the type map for that KeyName
  TVec<THash<TStr, TType> > TypeMaps;
  TType DefaultType; // default float

  // delimiter
  char FileDelimiter; // default ','


  TBool IsTimeStr; // true if the time needs to be formatted (ie not a long)
  TStr TimeFormatter; // the format string to convert the time into a tm
  TBool HasTime; //time column is specified

public:
  TSchema(const TStr & filename) : KeyNames(), KeyNamesToIndex(), Dirs(), Cols(), 
    TypeMaps(), DefaultType(FLOAT), FileDelimiter(','),
    IsTimeStr(false), TimeFormatter(), HasTime(false) {
      ReadSchemaFile(filename);
    }

  // Convert the given time string to a TTime using the given time formatter.
  // If there is no time formatter specified readd the string straight as a
  //long long.
  TTime ConvertTime(const TStr & time_val) const;

  TType GetType(const TStrV & IdVec);

private:
  // Read the given schema file and update the TSchema fields as according
  // to the given schema format
  void ReadSchemaFile(const TStr & filename);

  // Checks to make sure the key name has not already been registered. Otherwise
  // place the key name into the KeyNames vector and update the mapping from
  // key name to KeyNames vector index. Return this index.
  TInt AddNewKeyName(const TStr & key_name);

  // Read the file hierarchy section. Until hitting end, read the section
  // line by line. If the line starts with DELIM:, set the last character
  // of the line as the delimiter. Otherwise, the section line corresponds
  // to a directory, so update the schema's directory information.
  // Return whether time has been set as one of the directories.
  bool ReadFileHierarchy(TFIn & instream);

  // Read the schema section. Until hitting end, read each row as defining a
  // column of the data file, updating ID and sensor data structures
  // accordingly.
  bool ReadDataSchema(TFIn & instream);

  // Read the types section, which associates a type with an identifier.
  void ReadTypes(TFIn & instream);

  // Read the time conversion section, which adds a custom time format string
  void ReadTimeConversion(TFIn & instream);

  // ------ Schema file read methods -------

  // Get lines from the file stream until finding a line that does not
  // begin with the comment character #. Return false if ran out of lines.
  static bool GetNextUncommentedLine(TFIn & instream, TStr & result);
  
  // Advance the instream by one line and decide what section header is
  // beginning. If the section header is valid and not used yet, mark
  // the section header as used, and return true. If the section header
  // is not valid throw an error. If there are no lines left return false.
  static bool GetNextSchemaSection(TFIn & instream, bool* completed_sections, 
    SchemaCategory & schema_category);

  // This method reads the next line and determines if it's the end of a 
  // section. It should be called while in the middle of a section.
  // Advance the instream by one line. If there are no more lines to read,
  // throw an error (we have not ended the section properly). If the
  // line read is an END, return true. Otherwise place the line into the
  // line variable and return false.
  static bool GetSectionLine(TFIn & instream, TStr & line);
};

#endif
