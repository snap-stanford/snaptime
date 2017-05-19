import os
import csv
import sys
def getType(val):
	if val[0] == 'f' or val[0] == 't':
		return "BOOLEAN"
	if val.isdigit():
		return "INTEGER"
	if val.replace(".", "").isdigit():
		return "FLOAT"
	return "STRING"

def getTypes(directory):
	types = []
	for fn in os.listdir(directory):
		full_fn = directory + "/" + fn
		with open(full_fn, 'rb') as dfile:
			if "_values.tsv" not in fn:
				tsvin = csv.reader(dfile, delimiter='\t')
				for row in tsvin:
					 type_v = "%s, %s \n" % (fn, getType(row[-1]))
					 print type_v
					 types.append(type_v)
					 break
	return types

def writeOutput(fn, data):
	with open(fn, 'w+') as ofile:
		for row in data:
			ofile.write(row)

if __name__ == "__main__":
	assert len(sys.argv) == 3, "wrong number of arguments"
	input_dir = sys.argv[1]
	output_fn = sys.argv[2]
	types = getTypes(input_dir)
	writeOutput(output_fn, types)