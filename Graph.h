#pragma once
#include <vector>

enum class NodeState { NORMAL, START, END, PATH, OPEN, CLOSED };

struct Edge {
  int targetId;
  float weight;
};

struct Node {
  int id;
  float x, y; // Coordenadas en espacio OpenGL [-1, 1]
  NodeState state = NodeState::NORMAL;
};

struct GraphData {
  std::vector<Node> nodes;
  std::vector<std::vector<Edge>> adj;
  int startId = -1;
  int endId = -1;
};
