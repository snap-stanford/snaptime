%{
        #define SWIG_FILE_WITH_INIT
        #include "solver.hpp"
%}
%include "numpy.i"
%init %{
    import_array();
%}


%include "typemaps.i"
%include "std_vector.i"
%include "eigen.i"
%eigen_typemaps(Eigen::MatrixXd)

namespace std {
   %template(IntList) vector<int>;
   %template(NumpyList) vector<Eigen::MatrixXd>;
   %template(Numpy2DList) vector<vector<Eigen::MatrixXd> >;
}

%include "solver.hpp"
