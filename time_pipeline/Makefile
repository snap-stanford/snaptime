CC = g++
CXXFLAGS += -std=c++98 -pg -Wall
LDFLAGS += -lrt

SNAP = ../../../snap/snap-core
GLIB = ../../../snap/glib-core

# File names
SOURCES = $(wildcard *.cpp)
 
opt: Snap.o SnapTime.o test_snap_time


# Main target
test_snap_time: test_snap_time.cpp Snap.o SnapTime.o
	g++ $(CXXFLAGS) -o test_snap_time test_snap_time.cpp Snap.o SnapTime.o -I$(GLIB) -I$(SNAP)

 
Snap.o: 
	g++ -c $(CXXFLAGS) $(SNAP)/Snap.cpp -I$(GLIB) -I$(SNAP)

# # To obtain object files
SnapTime.o: *.cpp Snap.o
	$(CC) -c $(CXXFLAGS) $< Snap.o -I$(GLIB) -I$(SNAP) -o $@
 
# To remove generated files
clean:
	rm -f test_snap_time *.o