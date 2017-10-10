CC = g++
CXXFLAGS += -std=c++11 -Wall
LDFLAGS += -lrt

SNAP = ../../../snap/snap-core
GLIB = ../../../snap/glib-core

TEST_LIBRARY = /usr/local/include/gtest/include/

TEST_DIR = /tests


DEPS = stime_parser.hpp stime_helper.hpp
OBJ = stime_parser.o stime_helper.o

opt: Snap.o SnapTime.o test_snap_time

Snap.o: 
	$(CC) -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

SnapTime.o:
	$(CC) -c $(CXXFLAGS) SnapTime.cpp -I$(GLIB) -I$(SNAP) -o $@

# test_snap_time: test_snap_time.cpp Snap.o SnapTime.o
# 	$(CC) $(CXXFLAGS) -o test_snap_time test_snap_time.cpp Snap.o SnapTime.o -I$(GLIB) -I$(SNAP)

stime_test: tests/stime_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/bin/stime_test tests/stime_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)

schema_test: tests/schema_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/schema_test tests/schema_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)

parser_test: tests/parser_test.cpp SnapTime.o Snap.o
	$(CC) $(CXXFLAGS) -o tests/parser_test tests/parser_test.cpp SnapTime.o Snap.o -lgtest -I$(GLIB) -I$(SNAP)


# %.o: %.c
# 	$(CC) -c $(CXXFLAGS) $< -I$(GLIB) -I$(SNAP) -o $@

# # File names
# SOURCES = $(wildcard *.cpp)
 
# # Main target
# test_snap_time: test_snap_time.cpp Snap.o SnapTime.o
# 	g++ $(CXXFLAGS) -o test_snap_time test_snap_time.cpp SnapTime.o -I$(GLIB) -I$(SNAP)

 


# # # To obtain object files
# SnapTime.o: *.cpp
# 	$(CC) -c $(CXXFLAGS) $< -I$(GLIB) -I$(SNAP) -o $@
 
# # To remove generated files
clean:
	rm -f test_snap_time *.o tests/schema_test tests/parser_test
