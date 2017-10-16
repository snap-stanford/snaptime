%module SnapTime
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%{
	#include "Snap.h"
	#include "SnapTime.hpp"
	#include "stime.hpp"
%}
%include "../../snap-python/swig/snap.i"
%include "Snap.h"
%include "SnapTime.hpp"
%include "stime.hpp"
%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<int>;
%template(StringPair) std::pair<std::string, std::string>;
%template(StringPairVector) std::vector<std::pair<std::string, std::string> >;
%template(DoubleVector) std::vector<double>;
%template(VectorVectorDouble) std::vector<std::vector<double> >;
