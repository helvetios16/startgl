CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -lGL -lglfw

TARGET = pathnode
SRC = main.cc Visualizer.cc Pathfinder.cc AStarPathfinder.cc \
      DijkstraPathfinder.cc BFSPathfinder.cc DFSPathfinder.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
