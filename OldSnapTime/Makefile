#
# Makefile for SWIG processing and package building of SnapTime
#	Use this Makefile to compile SnapTime from scratch
#
# Build instructions for Mac OS X and Linux:
#		make
#		make dist
# 
# Build instructions for Windows:
#	- run swig in Cygwin to generate fillData_wrap.cxx and fillData.py
#		make swig-win
#	- build SnapTime solution in Visual Studio to generate _fillData.pyd
#		TODO, code below is not working
#		- open snappy-VS12
#		- add VC++ include path for snap-core, glib-core, snap-adv
#		  (or 'subst S: <path_to_snap>')
#		- build
#	- run setup.py in Cygwin to generate the package,
#	  NOT OVER SSH, but locally
#		make dist-win

# set the path to your SNAP directory here
GITDIR = ../../snap
SNAPDIR = $(GITDIR)/$(SNAP)
GLIBDIR = $(GITDIR)/$(GLIB)
SNAPADVDIR = $(GITDIR)/$(SNAPADV)
SNAPEXPDIR = $(GITDIR)/$(SNAPEXP)
CPPDIR = cpp
NUMPYDIR = $(shell python-config --includes) $(shell python -c "import numpy; print '-I' + numpy.get_include()")

# include compilation parameters
include $(GITDIR)/Makefile.config
include Makefile.config

all: snaptime_helper.py _snaptime_helper.so

snaptime_helper_wrap.cxx : $(CPPDIR)/snaptime_helper.i
	swig $(SWIGFLAGS) -python -c++ -w302,312,317,325,362,383,384,389,401,503,508,509 -O -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR) $(CPPDIR)/snaptime_helper.i

snaptime_helper_wrap.o: snaptime_helper_wrap.cxx
	g++ -Wall -O2 -pg -ftree-vectorize $(CXXFLAGS) -c $(CPPDIR)/snaptime_helper_wrap.cxx -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR) -I/usr/include/python2.6 -I/usr/include/python2.7 -I/usr/lib/python2.7/dist-packages/numpy/core/include -I$(NUMPYDIR)


solver.o: $(CPPDIR)/solver.cpp
	$(CC) -Wall -O2 -pg -ftree-vectorize $(CXXFLAGS) -c $(CPPDIR)/solver.cpp -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR)

fillData.o: $(CPPDIR)/fillData.cpp
	$(CC) -Wall -O2 -pg -ftree-vectorize $(CXXFLAGS) -c $(CPPDIR)/fillData.cpp -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR)

create_data.o: $(CPPDIR)/create_data.cpp
	$(CC) $(CXXFLAGS) -Wall -O2 -pg -ftree-vectorize -c $(CPPDIR)/create_data.cpp -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR)

Snap.o:
	$(CC) $(CXXFLAGS) -c $(SNAPDIR)/Snap.cpp -I$(SNAPDIR) -I$(SNAPADVDIR) -I$(GLIBDIR) -I$(NUMPYDIR)

_snaptime_helper.so: snaptime_helper_wrap.o Snap.o fillData.o create_data.o solver.o
	g++ $(LDFLAGS) -o _snaptime_helper.so snaptime_helper_wrap.o fillData.o create_data.o solver.o Snap.o $(LIBS)

snaptime_helper.py: snaptime_helper_wrap.cxx

install: setup.py _snaptime_helper.so snaptime_helper.py
	cp $(CPPDIR)/snaptime_helper.py .
	python setup.py install --user
	rm -f *.o *_wrap.cxx *.pyc
#	sudo python setup-snaptime.py install

dist: setup.py _snaptime_helper.so snaptime_helper.py
	#cp $(MANIFEST) MANIFEST
	cp $(CPPDIR)/snaptime_helper.py .
	python setup.py sdist
	rm -f *.o *_wrap.cxx *.pyc

swig-win: fillData_wrap.cxx

install-win: setup.py fillData.py _fillData.pyd
	/cygdrive/c/Python27/python.exe setup.py install

dist-win: setup.py fillData.py _fillData.pyd
	cp $(MANIFEST) MANIFEST
	/cygdrive/c/Python27/python.exe setup.py sdist
	chmod 0644 dist/*.zip

clean:
	rm -f *.o *_wrap.cxx _*.so *.pyc snap.py _snap.*

