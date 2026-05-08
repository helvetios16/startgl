#include "PathfinderUtils.h"
#include <vector>

void DFSPathfinder::solve(GraphData &graph) {
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    applyFrame(graph, frames.back());
}

std::vector<SearchFrame> DFSPathfinder::solveSteps(const GraphData &source) {
  GraphData graph = source;
  Pathfinder::resetGraph(graph);

  std::vector<SearchFrame> frames;
  frames.push_back(makeFrame(graph));

  if (graph.startId == -1 || graph.endId == -1)
    return frames;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  std::vector<bool> visited(n, false);
  std::vector<int> stack;

  for (int i = 0; i < n; ++i)
    info[i].id = i;

  stack.push_back(graph.startId);
  visited[graph.startId] = true;

  bool found = false;
  while (!stack.empty()) {
    int u = stack.back();
    stack.pop_back();

    if (graph.nodes[u].state == NodeState::NORMAL) {
      graph.nodes[u].state = NodeState::CLOSED;
      frames.push_back(makeFrame(graph));
    }

    if (u == graph.endId) {
      found = true;
      break;
    }

    for (const auto &edge : graph.adj[u]) {
      int v = edge.targetId;
      if (visited[v])
        continue;

      visited[v] = true;
      info[v].parentId = u;
      graph.nodes[v].parentId = u;
      stack.push_back(v);

      if (graph.nodes[v].state == NodeState::NORMAL) {
        graph.nodes[v].state = NodeState::OPEN;
        frames.push_back(makeFrame(graph));
      }
    }
  }

  if (found) {
    appendPathFrames(graph, info[graph.endId].parentId, info, frames);
  }
  return frames;
}
