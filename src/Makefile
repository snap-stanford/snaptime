CC = g++
CXXFLAGS += -std=c++11 -Wall
LDFLAGS += -shared

SNAP = ../../../snap/snap-core
GLIB = ../../../snap/glib-core

TEST_LIBRARY = /usr/local/include/gtest/include/

TEST_DIR = /tests


DEPS = stime_parser.hpp stime_helper.hpp
OBJ = stime_parser.o stime_helper.o

SWIGFLAGS += -D_CMPWARN -D__stdcall -DSW_SNAPPY -D_OPENMP -DNONUMPY -DUSE_OPENMP -DGCC_ATOMIC

opt: Snap.o SnapTime.o test_snap_time

Snap.o: 
	$(CC) -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

SnapTime.o:
	$(CC) -c $(CXXFLAGS) SnapTime.cpp -I$(GLIB) -I$(SNAP) -o $@

SnapTime_wrap.cxx: SnapTime.i
	swig $(SWIGFLAGS) -python -c++ -w302,312,317,325,362,383,384,389,401,503,508,509 -O -I$(GLIB) -I$(SNAP) SnapTime.i

SnapTime_wrap.o: SnapTime_wrap.cxx
	g++ -Wall -O2 -pg -ftree-vectorize $(CXXFLAGS) -c SnapTime_wrap.cxx -I$(GLIB) -I$(SNAP) -I/usr/include/python2.6 -I/usr/include/python2.7

_SnapTime.so: SnapTime_wrap.o Snap.o SnapTime.o
	g++ $(LDFLAGS) -lpython -dynamclib -o _SnapTime.so SnapTime_wrap.o Snap.o SnapTime.o

SnapTime.py: SnapTime_wrap.cxx

# test_snap_time: test_snap_time.cpp Snap.o SnapTime.o
# 	$(CC) $(CXXFLAGS) -o test_snap_time test_snap_time.cpp Snap.o SnapTime.o -I$(GLIB) -I$(SNAP)

stime_test: tests/stime_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/bin/stime_test tests/stime_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)

schema_test: tests/schema_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/schema_test tests/schema_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)

parser_test: tests/parser_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/bin/parser_test tests/parser_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)

dist: setup.py _SnapTime.so SnapTime.py
	python setup.py install --user

 
# # To remove generated files
clean:
	rm -f *.o tests/bin/*
	rm -rf tests/Scratch/
	rm -f *_wrap.cxx _*.so
