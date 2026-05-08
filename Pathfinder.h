#pragma once
#include "Graph.h"
#include <vector>

struct SearchFrame {
  std::vector<NodeState> states;
  std::vector<int> parents;
};

class IPathfinder {
public:
  virtual ~IPathfinder() = default;
  virtual void solve(GraphData &graph) = 0;
  virtual std::vector<SearchFrame> solveSteps(const GraphData &graph) = 0;
};

class AStarPathfinder : public IPathfinder {
public:
  void solve(GraphData &graph) override;
  std::vector<SearchFrame> solveSteps(const GraphData &graph) override;
};

class DijkstraPathfinder : public IPathfinder {
public:
  void solve(GraphData &graph) override;
  std::vector<SearchFrame> solveSteps(const GraphData &graph) override;
};

class BFSPathfinder : public IPathfinder {
public:
  void solve(GraphData &graph) override;
  std::vector<SearchFrame> solveSteps(const GraphData &graph) override;
};

class DFSPathfinder : public IPathfinder {
public:
  void solve(GraphData &graph) override;
  std::vector<SearchFrame> solveSteps(const GraphData &graph) override;
};

class Pathfinder {
public:
  static void resetGraph(GraphData &graph);
};
