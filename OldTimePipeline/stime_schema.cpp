// schema headers
const char* const kFileHierachyHeader = "START FileHierarchy";
const char* const kSchemaHeader = "START Schema";
const char* const kTimeConversionHeader = "START TimeConvert";
const char* const kTypesHeader = "START SchemaTypes";
const char* const kFileHierachyFooter = "END FileHierarchy";
const char* const kSchemaFooter = "END Schema";
const char* const kTimeConversionFooter = "END TimeConvert";
const char* const kTypesFooter = "END SchemaTypes";
// Column Behaviors
const char* const kNull = "NULL";
const char* const kTime = "TIME";
const char* const kID = "ID";
const char* const kSensor = "SENSOR";
// types
const char* const kInteger = "INTEGER";
const char* const kBoolean = "BOOLEAN";
const char* const kFloat = "FLOAT";
const char* const kString = "STRING";
const char* const kDefault = "DEFAULT";
const char* const kDelimLabel = "DELIM:";



namespace {
// GetNextSchemaLine reads in a line from the file, skipping comments and empty lines
bool GetNextSchemaLine(std::ifstream & infile, std::string & line) {
	while(std::getline(infile, line)) {
		line = TCSVParse::trim(line);
		if (line == "" || line[0] == '#') {
			continue; //skip line if empty or if commented out
		} else {
			return true; // found line that isn't bogus
		}
	}
	return false; //no valid lines
}

} // end anonymous namespace

// Read in the schema file section by section
void TTSchema::ReadSchemaFile(TStr filename) {
	defaultType = FLOAT;
	HasTime = false;
	std::ifstream infile(filename.CStr()); // open file
	AssertR(infile.is_open(), "could not open schema file");
	std::string line;
	bool hierarchyComplete = false, schemaComplete = false, timeConversionComplete = false, schemaTypes = false;
	while(GetNextSchemaLine(infile, line)) {
		if (line == kFileHierachyHeader) {
			AssertR(!hierarchyComplete, "hierarchy section listed twice");
			hierarchyComplete = true;
			ReadFileHierarchy(infile);
		} else if (line == kSchemaHeader) {
			AssertR(!schemaComplete, "schema section listed twice");
			schemaComplete = true;
			ReadDataSchema(infile);
		} else if (line == kTimeConversionHeader) {
			AssertR(!timeConversionComplete, "time conversion section listed twice");
			timeConversionComplete = true;
			ReadTimeConversion(infile);
		} else if (line == kTypesHeader) {
			AssertR(!schemaTypes, "schema types section listed twice");
			schemaTypes = true;
			ReadSchemaTypes(infile);
		} else {
			AssertR(false, "Invalid start of schema section");
		}
	}
	AssertR(HasTime, "must have a time value");
	AssertR(schemaComplete, "must have a schema section");
	if (!hierarchyComplete) {
		Dirs.Add(kNull); // add default file hierarchy: just bare files
	}
	divideFileSchemaByType();
}

// ConvertTime converts the string timeVal into a uint64 based on the time formatted string
TTime TTSchema::ConvertTime(TStr timeVal) const {
	if (!IsTimeStr) return timeVal.GetUInt64();
	struct tm ts;
	memset(&ts, 0,  sizeof(ts));
	strptime(timeVal.CStr(), TimeFormatter.CStr(), &ts);
	time_t t = mktime(&ts);
	return (TTime) t;
}

// Add the given KeyName to the list of KeyNames and the KeyName hash
void TTSchema::AddKeyName(TStr & KeyName) {
	AssertR(!KeyNameToIndex.IsKey(KeyName), "Key already exists in ID Name Hash");
	TInt new_index = KeyNames.Len();
	KeyNames.Add(KeyName);
	KeyNameToIndex.AddDat(KeyName, new_index);
}

//  ReadTimeConversion reads in the time conversion
void TTSchema::ReadTimeConversion(std::ifstream & infile) {
	std::string line;
	bool success = GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success, "Incomplete Time Conversion Section");
	IsTimeStr = true;
	TimeFormatter = TStr(line.c_str());
	success = GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success && line == kTimeConversionFooter, "Expected end of section");
}

void TTSchema::ReadFileHierarchy(std::ifstream & infile) {
	std::string line;
	bool success = GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success, "Expected hierarchy list or end of section");
	if (line == kFileHierachyHeader) {
		Dirs.Add(kNull);
		return;
	}
	// option for delimiter
	if (TStr(line.c_str()).IsPrefix(kDelimLabel)) {
		std::string delim = line.substr(strlen(kDelimLabel));
		AssertR(delim.length() == 1, "delimiter must be only one character");
		FileDelimiter = delim[0];
		success = GetNextSchemaLine(infile, line); // read the directory list
		AssertR(success, "Expected hierarchy list or end section");
		if (line == kFileHierachyFooter) {
				Dirs.Add(kNull);
				return;
		}
	}
	Dirs = TCSVParse::readCSVLine(line, ',', true);
	for (int i=0; i<Dirs.Len(); i++) {
		TStr dir = Dirs[i];
		if (dir == kTime) {
			AssertR(!HasTime, "specified a time column twice");
			HasTime = true;
		} else if(dir != kNull) {
			AddKeyName(dir);
		}
	}
	// read END FileHierarchy:
	success = GetNextSchemaLine(infile, line); // read "END"
	AssertR(success && line == kFileHierachyFooter, "must end hierarchy section");
}

void TTSchema::ReadDataSchema(std::ifstream & infile) {
	std::string line;
	bool success = GetNextSchemaLine(infile, line);
	AssertR(success, "Incomplete file hierarchy section");
	int rowNum = 0;
	int sensorNum = 0;
	while (line != kSchemaFooter) {
		TVec<TStr> row = TCSVParse::readCSVLine(line, ',', true);
		AssertR(row.Len() == 2, TStr("Invalid hierarchy row: ") + TStr(line.c_str()));
		TStr val = row[0];
		TStr type_str = row[1];
		TColType type;
		if (type_str == kTime) {
			type = TIME;
			AssertR(!HasTime, "time defined twice");
			HasTime = true;
		} else if (type_str == kNull) {
			type = NO_ID;
		} else if (type_str == kID) {
			type = ID;
			AddKeyName(val);
		} else if (type_str == kSensor) {
			type = SENSOR;
			sensorNum++;
		}
		else AssertR(false, TStr("invalid column type for file hierarchy: ") + TStr(line.c_str()));
		TPair<TStr, TColType> col(val, type);
		FileSchema.Add(col);
		rowNum++;
		AssertR(GetNextSchemaLine(infile, line), "ended file hierarchy section without proper footer");
	}
	AssertR(sensorNum > 0, "Must have at least one sensor value");
	if (KeyNameToIndex.IsKey(TStr(kSensor))) {
		//Sensor is already a key, so we have an ID field specifying the type of sensor
		AssertR(sensorNum == 1, "Cannot have multiple sensors in one line if SENSOR is an KeyName");
	} else {
		TStr sensor_id = TStr(kSensor);
		AddKeyName(sensor_id);
	}
}

void TTSchema::ReadSchemaTypes(std::ifstream & infile) {
	std::string line;
	bool success = GetNextSchemaLine(infile, line);
	AssertR(success, "Incomplete Schema Type section");
	while (line != "END SchemaTypes") {
		TVec<TStr> row = TCSVParse::readCSVLine(line, ',', true);
		AssertR(row.Len()==2, "Invalid row in schema types section");
		TStr sensorName = row[0];
		// get sensor type
		TStr str_sensor_type = row[1];
		TType sensor_type;
		if (str_sensor_type == kFloat) sensor_type= FLOAT;
		else if (str_sensor_type == kInteger) sensor_type= INTEGER;
		else if (str_sensor_type == kBoolean) sensor_type= BOOLEAN;
		else if (str_sensor_type == kString) sensor_type= STRING;
		else AssertR(false, "invalid type in schema types section");

		if (sensorName == kDefault) defaultType = sensor_type;
		else {
			//add type for specific sensor
			AssertR(!SensorType.IsKey(sensorName), "sensor specifed twice in sensor type section");
			SensorType.AddDat(sensorName, sensor_type);
		}
		AssertR(GetNextSchemaLine(infile, line), "ended sensor type section without END");
	}
}


void TTSchema::divideFileSchemaByType() {
	for (int i = 0; i < FileSchema.Len(); i++) {
		TPair<TStr, TColType> col = FileSchema[i];
		FileSchemaIndexList[(int)col.GetVal2()].Add(TInt(i));
	}
}

void TTSchema::PrintSchema() const {
	std::cout << "KeyNames" << std::endl;
	for (int i=0; i<KeyNames.Len(); i++) {
		std::cout << KeyNames[i].CStr() << ", ";
	}
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "KeyNameToIndex" << std::endl;
	std::cout << "{";
	for (THashKeyDatI<TStr, TInt> it = KeyNameToIndex.BegI(); it != KeyNameToIndex.EndI(); it++) {
		std::cout << it.GetKey().CStr() << ": " << it.GetDat().Val << ", ";
	}
	std::cout << "}" << std::endl;

	std::cout << std::endl;
	std::cout << "Dirs" << std::endl;
	for (int i=0; i<Dirs.Len(); i++) {
		std::cout << Dirs[i].CStr() << ", ";
	}
	std::cout << std::endl;
	std::cout << "FileSchema" << std::endl;
	for (int i=0; i<FileSchema.Len(); i++) {
		std::cout << "(" << FileSchema[i].GetVal1().CStr() << "," << FileSchema[i].GetVal2() << "), ";
	}
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "SensorType" << std::endl;
	std::cout << "{";
	for (THashKeyDatI<TStr, TType> it = SensorType.BegI(); it != SensorType.EndI(); it++) {
		std::cout << it.GetKey().CStr() << ": " << it.GetDat() << ", ";
	}
	std::cout << "}" << std::endl;

	std::cout << std::endl;
	std::cout << "Default Type: " << defaultType << std::endl;
	std::cout << "IsTimeStr: " << IsTimeStr << std::endl;
	std::cout << "TimeFormatter: " << TimeFormatter.CStr() << std::endl;
}
