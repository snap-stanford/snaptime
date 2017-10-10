#include "stime_schema.hpp"

const char * const FILE_HIERARCHY_HDR = "START FileHierarchy";
const char * const SCHEMA_HDR = "START Schema";
const char * const TYPES_HDR = "START SchemaTypes";
const char * const TIME_CONVERT_HDR = "START TimeConvert";

// Reading Schema
void TSchema::ReadSchemaFile(const TStr & filename) {
  TFIn instream(filename);
  bool completed_sections[NUM_SCHEMA_SECTIONS];
  memset(completed_sections, 0, sizeof(completed_sections));
  SchemaCategory schema_category;
  bool has_time = false;
  while(GetNextSchemaSection(instream, completed_sections, schema_category)) {
    switch (schema_category) {
      case FILE_HIERARCHY:
        has_time |= ReadFileHierarchy(instream);
        break;
      case SCHEMA:
        has_time |= ReadDataSchema(instream);
        break;
      case TYPES:
        ReadTypes(instream);
        break;
      case TIME_CONVERT:
        ReadTimeConversion(instream);
        break;
    }
  }
  AssertR(has_time, "Data points must set time to be valid");
  AssertR(completed_sections[1], "must have a data section");
}

bool TSchema::GetNextUncommentedLine(TFIn & instream, TStr & result) {
  while(instream.GetNextLn(result)) {
    if (result.Len() != 0 && !result.StartsWith("#")) return true;
  }
  return false;
}

bool TSchema::GetNextSchemaSection(TFIn & instream, bool* completed_sections, 
  SchemaCategory & schema_category) {
  TStr section_hdr;
  SchemaCategory temp = FILE_HIERARCHY;
  int section_index = -1;
  // no more lines left
  if (!GetNextUncommentedLine(instream, section_hdr)) return false;
  section_hdr = section_hdr.GetTrunc(); // trim whitespace
  if (section_hdr == TStr(FILE_HIERARCHY_HDR)) {
    temp = FILE_HIERARCHY;
    section_index = 0;
  } else if (section_hdr == TStr(SCHEMA_HDR)) {
    temp = SCHEMA;
    section_index = 1;
  } else if (section_hdr == TStr(TYPES_HDR)) {
    AssertR(completed_sections[SCHEMA], "Schema section must be listed before the types section");
    temp = TYPES;
    section_index = 2;
  } else if (section_hdr == TStr(TIME_CONVERT_HDR)) {
    temp = TIME_CONVERT;
    section_index = 3;
  } else {
    AssertR(false, "Invalid start of schema section");
  }
  if (completed_sections[section_index]) {
    AssertR(false, "Schema header " + section_hdr + " has already been used");
  }
  completed_sections[section_index] = true;
  schema_category = temp;
  return true;
}

bool TSchema::GetSectionLine(TFIn & instream, TStr & line) {
  AssertR(GetNextUncommentedLine(instream, line),
    "Schema section needs to end with END");
  return line != TStr("END");
}

TInt TSchema::AddNewKeyName(const TStr & key_name) {
  AssertR(!KeyNamesToIndex.IsKey(key_name), "key name "+ key_name
          + " has already been used ");
  TInt index = KeyNames.Len();
  KeyNames.Add(key_name);
  KeyNamesToIndex.AddDat(key_name, index);
  TypeMaps.Add(THash<TStr, TType>());
  return index;
}

bool TSchema::ReadFileHierarchy(TFIn & instream) {
  bool has_time = false;
  TStr schema_line;
  while (GetSectionLine(instream, schema_line)) {
    // section is still active
    if (schema_line.StartsWith("DELIM:")) {
      FileDelimiter = schema_line.LastCh();
      continue;
    } 
    TStrV directories;
    schema_line.SplitOnAllCh(',', directories);
    for (int i=0; i<directories.Len(); i++) {
      const TStr & directory = directories[i];
      TStr dir_name = directory.GetTrunc();
      TInt index;
      TKeyType behavior;
      if (dir_name == "NULL") {
        index = -1;
        behavior = NO_ID;
      } else if (dir_name == "TIME") {
        has_time = true;
        index = -1;
        behavior = TIME;
      } else {
        behavior = ID;
        index = AddNewKeyName(dir_name);
      }
      Dirs.Add({behavior, index});
    }
  }
  // If there are no directories set, add null
  if (Dirs.Empty()) Dirs.Add({NO_ID, -1});
  return has_time;
}

bool TSchema::ReadDataSchema(TFIn & instream) {
  bool has_time = false;
  TStr schema_line;
  while (GetSectionLine(instream, schema_line)) {
    TStrV col;
    schema_line.SplitOnAllCh(',', col);
    AssertR(col.Len() == 2, "Schema section must have lines with 2 fields");
    TStr name = col[0].GetTrunc();
    TStr behavior = col[1].GetTrunc();
    TKeyType col_behavior;
    TInt col_index;
    if (behavior == "TIME") {
      has_time = true;
      col_behavior = TIME;
      col_index = -1;
    } else if (behavior == "ID") {
      col_behavior = ID;
      col_index = AddNewKeyName(name);
    } else if (behavior == "SENSOR") {
      col_behavior = SENSOR;
      col_index = SensorNames.Len();
      SensorNames.Add(name);
    } else {
      AssertR(false, "invalid line in SCHEMA section");
    }
    Cols.Add({col_behavior, col_index});
  }
  AddNewKeyName("SENSOR");
  AssertR(SensorNames.Len() > 0, "must have at least one sensor value");
  return has_time;
}

void TSchema::ReadTypes(TFIn & instream) {
  TStr schema_line;
  while(GetSectionLine(instream, schema_line)) {
    TStrV col;
    schema_line.SplitOnAllCh(',', col);
    AssertR(col.Len() >= 2, "Schema type section must have lines with 2 or 3 fields");
    TStr name = col[0].GetTrunc();
    TStr key_name, type;
    if (name == "DEFAULT") {
      type = col[1].GetTrunc();
    } else {
      AssertR(col.Len() == 3, "Schema type specification must have line with 3 fields");
      key_name = col[1].GetTrunc();
      type = col[2].GetTrunc();
    }
    TType id_type;
    if (type == "BOOLEAN") {
      id_type = BOOLEAN;
    } else if (type == "INTEGER") {
      id_type = INTEGER;
    } else if (type == "FLOAT") {
      id_type = FLOAT;
    } else if (type == "STRING") {
      id_type = STRING;
    } else {
      AssertR(false, "undefined type " + type);
    }
    if (name == "DEFAULT") {
      DefaultType = id_type;
    } else {
      AssertR(KeyNamesToIndex.IsKey(key_name), "undefined key name in type section");
      TInt index = KeyNamesToIndex.GetDat(key_name);
      TypeMaps[index].AddDat(name, id_type);
    }
  }
}

void TSchema::ReadTimeConversion(TFIn & instream) {
  TStr schema_formatter;
  AssertR(GetSectionLine(instream, schema_formatter), 
    "time convert section must define a format string");
  IsTimeStr = true;
  TimeFormatter = schema_formatter;
  AssertR(!GetSectionLine(instream, schema_formatter),
    "time converter needs to end with an END");
}

// Schema getter and transform methods
TTime TSchema::ConvertTime(const TStr & time_val) const {
  if ( !IsTimeStr) return time_val.GetUInt64();
  struct tm ts;
  memset(&ts, 0,  sizeof(ts));
  strptime(time_val.CStr(), TimeFormatter.CStr(), &ts);
  time_t t = mktime(&ts);
  return (TTime) t;
}

TType TSchema::GetType(const TStrV & IdVec) {
  AssertR(IdVec.Len() == TypeMaps.Len(), "invalid ID vector");
  // start from back of type specifier looking for types
  for (int i=TypeMaps.Len()-1; i>=0; i--) {
    TStr IdName = IdVec[i];
    if (TypeMaps[i].IsKey(IdName)) {
      return TypeMaps[i].GetDat(IdName);
    }
  }
  return DefaultType;
}