// TODO: not sure if this matters, but using format string from user could be a security bug
TTime TTSchema::ConvertTime(TStr timeVal) const {
	if (!IsTimeStr) {
		return timeVal.GetUInt64();
	}
	
	struct tm ts;
	strptime(timeVal.CStr(), TimeFormatter.CStr(), &ts);
	time_t t = mktime(&ts);
	return (TTime) t;
}

void TTSchema::PrintSchema() const {
	std::cout << "IdNames" << std::endl;
	for (int i=0; i<IdNames.Len(); i++) {
		std::cout << IdNames[i].CStr() << ", ";
	}
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "IDName_To_Index" << std::endl;
	std::cout << "{";
	for (THashKeyDatI<TStr, TInt> it = IDName_To_Index.BegI(); it != IDName_To_Index.EndI(); it++) {
		std::cout << it.GetKey().CStr() << ": " << it.GetDat().Val << ", ";
	}
	std::cout << "}" << std::endl;

	std::cout << std::endl;
	std::cout << "Dirs" << std::endl;
	for (int i=0; i<Dirs.Len(); i++) {
		std::cout << Dirs[i].CStr() << ", ";
	}
	std::cout << std::endl;

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

bool TTSchema::GetNextSchemaLine(std::ifstream & infile, std::string & line) {
	while(std::getline(infile, line)) {
		line = TCSVParse::trim(line);
		std::cout << line << std::endl;
		if (line == "" || line[0] == '#') {
			continue; //skip line if empty or if commented out
		} else {
			std::cout << "debug line " << line << std::endl;
			return true; // found line that isn't bogus
		}
	}
	return false; //no valid lines
}

void TTSchema::AddIDName(TStr & IdName) {
	AssertR(!IDName_To_Index.IsKey(IdName), "Key already exists in ID Name Hash");
	TInt new_index = IdNames.Len();
	IdNames.Add(IdName);
	IDName_To_Index.AddDat(IdName, new_index);
}

void TTSchema::ReadTimeConversion(std::ifstream & infile) {
	std::string line;
	bool success = TTSchema::GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success, "Incomplete Time Conversion Section");
	IsTimeStr = true;
	TimeFormatter = TStr(line.c_str());
	success = TTSchema::GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success && line == "END TimeConvert", "Expected 'END TimeConvert'");
}

void TTSchema::ReadFileHierarchy(std::ifstream & infile) {
	std::string line;
	bool success = TTSchema::GetNextSchemaLine(infile, line); // read the directory list
	AssertR(success, "Expected hierarchy list or 'END FileHierarchy'");
	if (line == "END FileHierarchy") {
		Dirs.Add("NULL");
		return;
	}
	// option for delimiter
	std::string delim_key("DELIM:");
	if (TStr(line.c_str()).IsPrefix(delim_key.c_str())) {
		std::cout << "has delimiter" << std::endl;
		std::string delim = line.substr(delim_key.size());
		AssertR(delim.length() == 1, "delimiter must be only one character");
		FileDelimiter = delim[0];
		success = TTSchema::GetNextSchemaLine(infile, line); // read the directory list
		AssertR(success, "Expected hierarchy list or 'END FileHierarchy'");
		if (line == "END FileHierarchy") {
				Dirs.Add("NULL");
				return;
		}
	}
	Dirs = TCSVParse::readCSVLine(line, ',', true);
	for (int i=0; i<Dirs.Len(); i++) {
		TStr dir = Dirs[i];
		if (dir == "TIME") {
			AssertR(!HasTime, "specified a time column twice");
			HasTime = true;
		} else if(dir != "NULL") {
			AddIDName(dir);
		}
	}
	// read END FileHierarchy:
	success = TTSchema::GetNextSchemaLine(infile, line); // read "END"
	AssertR(success && line == "END FileHierarchy", "'END FileHierarchy:' must appear as footer for hierarchy section");
}

void TTSchema::ReadDataSchema(std::ifstream & infile) {
	std::string line;
	bool success = TTSchema::GetNextSchemaLine(infile, line);
	AssertR(success, "Incomplete file hierarchy section");
	int rowNum = 0;
	int sensorNum = 0;
	while (line != "END Schema") {
		TVec<TStr> row = TCSVParse::readCSVLine(line, ',', true);
		AssertR(row.Len() == 2, TStr("Invalid hierarchy row: ") + TStr(line.c_str()));
		TStr val = row[0];
		TStr type_str = row[1];
		TColType type;
		if (type_str == "TIME") {
			type = TIME;
			AssertR(!HasTime, "TIME defined twice");
			HasTime = true;
		} else if (type_str == "NULL") {
			type = NO_ID;
		} else if (type_str == "ID") {
			type = ID;
			AddIDName(val);
		} else if (type_str == "SENSOR") {
			type = SENSOR;
			sensorNum++;
		}
		else AssertR(false, TStr("invalid column type for file hierarchy: ") + TStr(line.c_str()));

		TPair<TStr, TColType> col(val, type);
		FileSchema.Add(col);
		rowNum++;
		AssertR(TTSchema::GetNextSchemaLine(infile, line), "ended file hierarchy section without END");
	}
	AssertR(sensorNum > 0, "Must have at least one SENSOR value");
	if (IDName_To_Index.IsKey(TStr("SENSOR"))) {
		//Sensor is already a key, so we have an ID field specifying the type of sensor
		AssertR(sensorNum == 1, "Cannot have multiple sensors in one line if SENSOR is an IDName");
	} else {
		TStr sensor_id = TStr("SENSOR");
		AddIDName(sensor_id);
	}
}

void TTSchema::ReadSchemaTypes(std::ifstream & infile) {
	std::string line;
	bool success = TTSchema::GetNextSchemaLine(infile, line);
	AssertR(success, "Incomplete Schema Type section");
	while (line != "END SchemaTypes") {
		TVec<TStr> row = TCSVParse::readCSVLine(line, ',', true);
		AssertR(row.Len()==2, "Invalid row in schema types section");
		TStr sensorName = row[0];
		// get sensor type
		TStr str_sensor_type = row[1];
		TType sensor_type;
		if (str_sensor_type == "FLOAT") sensor_type= FLOAT;
		else if (str_sensor_type == "INTEGER") sensor_type= INTEGER;
		else if (str_sensor_type == "BOOLEAN") sensor_type= BOOLEAN;
		else if (str_sensor_type == "STRING") sensor_type= STRING;
		else AssertR(false, "invalid type in schema types section");

		if (sensorName == "DEFAULT") defaultType = sensor_type;
		else {
			//add type for specific sensor
			AssertR(!SensorType.IsKey(sensorName), "sensor specifed twice in sensor type section");
			SensorType.AddDat(sensorName, sensor_type);
		}
		AssertR(TTSchema::GetNextSchemaLine(infile, line), "ended sensor type section without END");
	}
}


void TTSchema::ReadSchemaFile(TStr filename) {
	defaultType = FLOAT;
	HasTime = false;
	std::ifstream infile(filename.CStr());
	AssertR(infile.is_open(), "could not open schema file");
	std::cout << "opened file" << std::endl;
	std::string line;
	bool hierarchyComplete = false;
	bool schemaComplete = false;
	bool timeConversionComplete = false;
	bool schemaTypes = false;
	while(TTSchema::GetNextSchemaLine(infile, line)) {
		if (line == "START FileHierarchy") {
			AssertR(!hierarchyComplete, "hierarchy section listed twice");
			hierarchyComplete = true;
			ReadFileHierarchy(infile);
		} else if (line == "START Schema") {
			std::cout << "starting schema" << std::endl;
			AssertR(!schemaComplete, "schema section listed twice");
			schemaComplete = true;
			ReadDataSchema(infile);
		} else if (line == "START TimeConvert") {
			AssertR(!timeConversionComplete, "time conversion section listed twice");
			timeConversionComplete = true;
			ReadTimeConversion(infile);

		} else if (line == "START SchemaTypes") {
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
		Dirs.Add("NULL"); // add default file hierarchy: just bare files
	}
	divideFileSchemaByType();
}

void TTSchema::divideFileSchemaByType() {
	for (int i = 0; i < FileSchema.Len(); i++) {
		TPair<TStr, TColType> col = FileSchema[i];
		FileSchemaIndexList[(int)col.GetVal2()].Add(TInt(i));
	}
}
