--------------
General:
The time series data must have the following structure. Each data point is, at 
its core, a "SENSOR" value paired with a "TIME" value. Each data point also 
can have associated "IDs" which determine the context in which the data point
occurred. We define a Key Name as a category under which ID Names fall. 
For example, a Key Name might be "MachineID" while an associated ID Name
would be "Machine001". A Sensor Name is also considered an ID name under the
Key Name "SENSOR"

We assume that the input is one or more raw data files, which may be split
into a directory structure. The file and/or directory names might have 
significance (for example, you could have a directory named "Machine001" and 
rightly assume that "Machine001" will be an ID for all data points in that 
directory).
---------------
Section: FileHierarchy
The file hierarchy specifies the directory structure for the raw data The 
section contains information on the delimiter, and the significance of 
file/directory names

Structure:
	START FileHierarchy
	DELIM:<delimiter>
	<DirectoryId>, <DirectoryId> ... 
	END

Notes:
<delimiter> should be one character	
<DirectoryId> can either be:
	NULL: no significance
	TIME: time value
	<Key Name>: the filename is the ID name associated with the given Key Name
If a DirectoryId is TIME, then TIME should not appear in the schema file. 
One DirectoryId should be specified for each directory starting from root, 
including the root directory and the data file.  So the line: "NULL, MachineID"
specifies a directory with file names that correspond to MachineIds
----------------
Section: Schema
The schema section specified the columns of an individual data file. Combined
with the directory names, the data must include exactly one TIME and at least 
one SENSOR value to fully define a data point.

Structure:
	START Schema
	<Name>, <IdType>
	...
	END

Notes:
	Name: can only be 1 word
	IdType can be:
		ID: the column value will be the ID name associated with the Key Name 
			specified by the Name value in the schema
		SENSOR: the column value is the value of the SENSOR called Name
		TIME: this column value represents the time
----------------
Section: SchemaTypes
This section contains type information about the data values in the data files.
Typing occurs from most specific identifier to least specific identifier.
Structure:
	START SchemaTypes
	DEFAULT, <Type>
	<IdName>, <KeyName>, <Type>
	....
	END SchemaTypes
Notes:
	Type: INTEGER, BOOLEAN, FLOAT, or STRING
	DEFAULT: the default type if not specified
	IdName: the ID name or sensor name
----------------
Section TimeConvert
Structure:
	START TimeConvert
	<Format String>
	END TimeConvert
Notes:
	FormatString is the string used to convert the time into a long long. 
	For example: %d.%m.%Y %H:%M:%S