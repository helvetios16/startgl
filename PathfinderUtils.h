#pragma once
#include "Pathfinder.h"
#include <cmath>
#include <vector>

struct NodeInfo {
  int id;
  float g;
  float h;
  int parentId;

  NodeInfo() : id(-1), g(1e9), h(0), parentId(-1) {}
  float f() const { return g + h; }
};

struct CompareF {
  bool operator()(const NodeInfo *a, const NodeInfo *b) {
    return a->f() > b->f();
  }
};

inline float getDistance(const Node &a, const Node &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

inline SearchFrame makeFrame(const GraphData &graph) {
  SearchFrame frame;
  frame.states.reserve(graph.nodes.size());
  frame.parents.reserve(graph.nodes.size());
  for (const auto &node : graph.nodes) {
    frame.states.push_back(node.state);
    frame.parents.push_back(node.parentId);
  }
  return frame;
}

inline void applyFrame(GraphData &graph, const SearchFrame &frame) {
  for (int i = 0; i < (int)graph.nodes.size(); ++i) {
    graph.nodes[i].state = frame.states[i];
    graph.nodes[i].parentId = frame.parents[i];
  }
}

inline void appendPathFrames(GraphData &graph, int endParent,
                             const std::vector<NodeInfo> &info,
                             std::vector<SearchFrame> &frames) {
  int curr = endParent;
  while (curr != -1 && curr != graph.startId) {
    graph.nodes[curr].state = NodeState::PATH;
    frames.push_back(makeFrame(graph));
    curr = info[curr].parentId;
  }
}
