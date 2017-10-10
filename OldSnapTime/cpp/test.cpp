#include "create_data.hpp"
#include <iostream>
#include "Eigen/Core"
#include "Eigen/Eigen"
#include "Snap.h"
#include <string>
#include <cstring>

//1398146400000
int main(int argc,char *argv[]){
    CreateData dt = CreateData();
    int dotpos = 0;
    std::string fname = "";
    while(argv[1][dotpos] != '.') {
        fname += argv[1][dotpos];
        dotpos++;
    }
    fname += ".bin";
    std::cout << argv[1] << " " << fname << "\n";
    const char *out = fname.c_str();
    dt.TSVtoBin(argv[1],out);
    //dt.TSVtoBin("test_file.tsv","outp.bin");
    //const TStr InFNm = "outp.bin";
    //TFIn FIn(InFNm);
    //dt.Load(FIn);
    //auto v = dt.fillData(1398146400000,3600000);
    //std::cout << v << "\n";
    //dt.parseData("test_file.tsv");
    //auto v = dt.fillData(1398146400000,3600000);
    //std::cout << dt.fillData(1398146400000,3600000) << "\n";
    return 0;
}
