%{
    #define SWIG_FILE_WITH_INIT
    #include "fillData.hpp"
%}

%include "typemaps.i"
%include "std_string.i"
%include "std_vector.i"
%include "eigen.i"
%eigen_typemaps(Eigen::MatrixXd)

%include "fillData.hpp"


