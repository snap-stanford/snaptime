%module SnapTime
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%{
	#include "Snap.h"
	#include "SnapTime.hpp"
	#include "stime.hpp"
        #include "stime_protos.hpp"
%}
%include "../../snap-python-64/swig/snap.i"
%apply double { ldouble } 
%include "Snap.h"
%include "SnapTime.hpp"
%include "stime.hpp"
%include "stime_protos.hpp"
%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<int>;
%template(StringPair) std::pair<std::string, std::string>;
%template(StringPairVector) std::vector<std::pair<std::string, std::string> >;
%template(DoubleVector) std::vector<double>;
%template(VectorVectorDouble) std::vector<std::vector<double> >;

%typemap(in) TLFlt& {
  $1 = new TLFlt((long double)PyFloat_AsDouble($input));
}

%typemap(freearg) TLFlt& {
   free($1);
}

%typemap(in) const TLFlt& {
  $1 = new TLFlt((long double)PyFloat_AsDouble($input));
}

%typemap(freearg) const TLFlt& {
   free($1);
}

%typemap(in) TLFlt {
  $1 = TLFlt((long double)PyFloat_AsDouble($input));
}

%typemap(in) TLFlt defaultValue {
  $1 = TLFlt((long double)PyFloat_AsDouble($input));
}

%typemap(out) TLFlt {
  $result = PyFloat_FromDouble((double) ($1.Val));
}

%typemap(out) TLFlt& {
  $result = PyFloat_FromDouble((double) ($1->Val));
}

%typecheck(SWIG_TYPECHECK_DOUBLE)
  float, double,
  const float &, const double &, TFlt, TFlt &, const TFlt, const TFlt&, TLFlt, TLFlt &, const TLFlt, const TLFlt&
{
  $1 = (PyFloat_Check($input) || PyInt_Check($input) || PyLong_Check($input)) ? 1 : 0;
}
