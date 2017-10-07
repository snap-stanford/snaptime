#ifndef STIME_PROTOS_H
#define STIME_PROTOS_H

// Time and data
typedef TUInt64 TTime; // time value
enum TType {BOOLEAN, STRING, INTEGER, FLOAT}; // Types of values
typedef TPair<TTime, TStr> TRawData; // pair of time to raw data
enum TKeyType {TIME, SENSOR, ID, NO_ID}; // Column behavior types

// Schema
#endif