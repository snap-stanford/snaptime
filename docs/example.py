'''
Tutorial on using SnapTime in python
To make: run make _SnapTime.so in src directory
'''

import SnapTime
config = SnapTime.SnapTimeConfiguration()
config.InputRawDirectory = "rawDir" # where is the data located
config.PrimaryDirectory = "primDir" # where to store primary
config.SymbolicDirectory = "symDir" # where to store links
config.SchemaFile = "schema.txt" # schema file
config.ModHierarchy = SnapTime.IntVector([13,17])
config.SymbolicSplit = SnapTime.StringVector(["IdentifierA", "IdentifierB"])
config.NumThreads = 30

# Generate indices. This will take a while
# rawDir and symDir should not exist before running these two commands
# these should only be run once
SnapTime.GeneratePrimaryDirectories(config)
SnapTime.SnapTime.GenerateSymbolicIndex(config)

# Run queries and inflate
q = SnapTime.QueryObject()
d_pair = ("IdentifierA", SnapTime.StringVector(["foo", "bar"]))
s_pair = ("IdentifierB", SnapTime.StringVector("baz", "qux"))
q.Queries = SnapTime.StringVectorVector([d_pair, s_pair])
q.InitialTimestamp = "2018-05-14 01:15:00.0" #example format string if you had formatter %Y-%m-%d %H:%M:%S.%f
q.FinalTimestamp = "2018-05-14 10:15:00.0"

result = SnapTime.Query(config, query, "result.bin", True) # run query, save, zero fill
result = SnapTime.LoadQuery("result.bin") # load prev binary
# Inflate Query
arr = SnapTime.InflateQuery(config, result, "2014-05-14 01:15:00.0", 3600, 0.1) # inflated array of doubles


