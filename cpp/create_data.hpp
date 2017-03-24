#ifndef CREATE_DATA_H_
#define CREATE_DATA_H_
#include <iostream>
#include <fstream>
#include "Eigen/Core"
#include "Eigen/Eigen"
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include "Snap.h"

class CreateData
{
    protected:
        TVec<TVec<TPair<TFlt,TFlt> > > signal_values;
        TVec<TStr> signal_names;
        TVec<TInt> signal_idx;
        int signal_count;
        struct heapData
        {
            int signal_index;
            int signal_position;
        };

    public:
        void Save(TSOut &Sout) {signal_names.Save(Sout);signal_idx.Save(Sout);signal_values.Save(Sout);}
        void Load(TSIn &Sin) {signal_names.Load(Sin);signal_idx.Load(Sin);signal_values.Load(Sin);}
        void TSVtoBin(const char *inp, const char *outp) {
            const TStr InFNm = inp;
            const TStr OutFNm = outp;
            TSsParser Ss(InFNm);
            TFOut FOut(OutFNm);
            int counter = 0;
            while (Ss.Next()) {
                if(strcmp(Ss[0],"bool") != 0 && strcmp(Ss[0],"float") != 0) {
                    continue;
                }
                signal_names.Add(TStr(Ss[0]));
                signal_idx.Add(TInt(atoi(Ss[1])));
                TVec<TPair<TFlt,TFlt> > data;
                for(int i = 2 ; i < Ss.Len(); ++i) {
                    char *token = strtok(Ss[i],",");
                    double timestamp = atof(token);
                    token = strtok(NULL,",");
                    double value = 0;
                    if(strcmp(Ss[0],"float") == 0) {
                        value = atof(token);
                    } else {
                        if (token[0] == 't') {
                            value = 1;
                        }
                    }
                    data.Add(TPair<TFlt,TFlt>(TFlt(timestamp),TFlt(value)));
                }
                signal_values.Add(data);
            }
            Save(FOut);
        }
        void parseData(std::string);
        Eigen::MatrixXd fillData(long long,int,int);
};
#endif
