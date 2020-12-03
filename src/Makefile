CXX = g++
LDFLAGS =
ROOTLIBS = $(shell root-config --libs)
LIBS = -lstdc++
CXXFLAGS = $(filter-out -std=c++17, $(shell root-config --cflags)) -std=c++11
TARGETS = readRawFile makeHistogram
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

.PHONY: all clean

all: $(TARGETS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(ROOTLIBS) $(LIBS) -o $@ -c $<

readRawFile: readRawFile.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(LIBS) $^

makeHistogram: makeHistogram.o
	$(CXX) -o $@ $(CXXFLAGS) $(LIBS) $(ROOTLIBS) $^

clean:
	$(RM) $(OBJECTS) $(TARGETS)

