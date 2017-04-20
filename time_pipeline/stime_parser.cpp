#include "stime_parser.hpp"
#include <algorithm>
#include <boost/tokenizer.hpp>

TVec<TStr> TSTimeParser::readCSVLine(std::string line, char delim) {
    // escape \, fields separated by ",", fields can be quoted with "
    boost::escaped_list_separator<char> sep( '\\', delim, '"' ) ;
    typedef boost::tokenizer< boost::escaped_list_separator<char> > boost_tokenizer;
    boost_tokenizer tok( line, sep );
    TVec<TStr> vec_line;
    for(boost_tokenizer::iterator beg= tok.begin(); beg!=tok.end(); ++beg)
    {
        vec_line.Add(TStr((*beg).c_str()));
    }
    return vec_line;
}

/// Should be ID, ID, ID... timestamp, value
void TSTimeParser::ReadEventFile(std::string filename) {
    std::ifstream infile(filename);
    // AssertR(inFile.is_open(), TStr("could not open file " + filename));
    std::string line;
    while(std::getline(infile, line)) {
        TTIdVec IDVector = TSTimeParser::readCSVLine(line);
        AssertR(IDVector.Len() >= 2, "must have at least a TS and value");

        TStr value = IDVector.Last();
        IDVector.DelLast();

        TTime ts = IDVector.Last().GetUInt64();
        IDVector.DelLast();

        TTRawData new_tv_pair = TTRawData(ts, value);

        if (!RawTimeData.IsKey(IDVector)) {
            TVec<TTRawData> new_time_data;
            new_time_data.Add(new_tv_pair);
            RawTimeData.AddDat(IDVector, new_time_data);
        } else {
            RawTimeData.GetDat(IDVector).Add(new_tv_pair);
        }
        CurrNumRecords++;
        if (CurrNumRecords >= MaxRecordCapacity) {
            FlushUnsortedData();
            CurrNumRecords = 0;
        }
    }
}

void TSTimeParser::FlushUnsortedData() {
    THash<TTIdVec, TVec<TTRawData> >::TIter it;
    time_t t = std::time(0);
    TUInt64 now = static_cast<uint64> (t);

    char new_dir_path[30]; //buffer to put new directory path
    new_dir_path[0] = '/';

    for (it = RawTimeData.BegI(); it != RawTimeData.EndI(); it++) {
        TTIdVec IdVec = it.GetKey();
        TVec<TTRawData> dat = it.GetDat();

        TUnsortedTime time_record(IdVec, dat);

        int prim_hash = IdVec.GetPrimHashCd(); //dirnames are based on hash of ids
        int sec_hash = IdVec.GetSecHashCd(); //dirnames are based on hash of ids
        sprintf(new_dir_path + 1, "%x_%x", prim_hash, sec_hash);
        TStr dir_path = Directory + TStr(new_dir_path);
        //create a directory for this record if it doesn't already exist
        if (!TDir::Exists(dir_path)) {
            AssertR(TDir::GenDir(dir_path), "failed to create directory");
        }

        TStr fn = dir_path + TStr('/') + now.GetStr() + TStr(".bin");
        TFOut outstream(fn);
        time_record.Save(outstream);
    }
}
 
// TODO: what if the vector is too big to hold in memory
void TSTimeParser::SortBucketedData(TStr DirPath, TType type) {
    TStrV FnV;
    // retrieve filenames
    TFFile::GetFNmV(DirPath, TStrV::GetV("bin"), false, FnV);
    TUnsortedTime unsorted_record;
    TTRawDataV BucketedData;
    for (int i=0; i<FnV.Len(); i++) {
        TStr filename = FnV[i];
        TFIn infile(filename);
        unsorted_record.Load(infile);
        BucketedData.AddV(unsorted_record.TimeData);
    }
    TTIdVec IDs = unsorted_record.KeyIds;
    RawDataCmp comparator;
    BucketedData.SortCmp(comparator);
    switch (type) {
        case BOOLEAN:
            TSTimeParser::WriteSortedData<TBool>(DirPath, IDs, BucketedData);
            break;
        case STRING:
            TSTimeParser::WriteSortedData<TStr>(DirPath, IDs, BucketedData);
            break;
        case INTEGER:
            TSTimeParser::WriteSortedData<TInt>(DirPath, IDs, BucketedData);
            break;
        default:
            TSTimeParser::WriteSortedData<TFlt>(DirPath, IDs, BucketedData);
            break;
    }
}


