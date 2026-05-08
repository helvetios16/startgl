#include "Pathfinder.h"

void Pathfinder::resetGraph(GraphData &graph) {
  for (auto &node : graph.nodes) {
    node.parentId = -1;
    if (node.state != NodeState::START && node.state != NodeState::END) {
      node.state = NodeState::NORMAL;
    }
  }
}
