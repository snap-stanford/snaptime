#include "Eigen/Core"
#include "Eigen/Eigen"
#include "create_data.hpp"

class FillData
{
    public:
        Eigen::MatrixXd createAndFillData(std::string,long long,int,int);
        void TSVToBin(std::string filename,std::string binaryfilename) {
            CreateData dt = CreateData();
            dt.TSVtoBin(filename.c_str(),binaryfilename.c_str());
        }
};
