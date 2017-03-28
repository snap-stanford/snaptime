#include "Eigen/Core"
#include "Eigen/Eigen"
#include "create_data.hpp"

class FillData
{
    public:
        Eigen::MatrixXd createAndFillData(std::string,long long,int,int);
        void TSVToBin(std::string filename) {
            CreateData dt = CreateData();
            int dotpos = 0;
            std::string fname = "";
            while(filename[dotpos] != '.') {
                fname += filename[dotpos];
                dotpos++;
            }
            fname += ".st";
            const char *out = fname.c_str();
            dt.TSVtoBin(filename.c_str(),out);
        }
};
