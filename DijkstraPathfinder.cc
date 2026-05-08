#include "PathfinderUtils.h"
#include <queue>

void DijkstraPathfinder::solve(GraphData &graph) {
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    applyFrame(graph, frames.back());
}

std::vector<SearchFrame>
DijkstraPathfinder::solveSteps(const GraphData &source) {
  GraphData graph = source;
  Pathfinder::resetGraph(graph);

  std::vector<SearchFrame> frames;
  frames.push_back(makeFrame(graph));

  if (graph.startId == -1 || graph.endId == -1)
    return frames;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  std::vector<bool> closedSet(n, false);
  std::priority_queue<NodeInfo *, std::vector<NodeInfo *>, CompareF> openSet;

  for (int i = 0; i < n; ++i)
    info[i].id = i;

  NodeInfo &start = info[graph.startId];
  start.g = 0;
  start.h = 0;
  openSet.push(&start);

  bool found = false;
  while (!openSet.empty()) {
    NodeInfo *currentInfo = openSet.top();
    openSet.pop();

    int u = currentInfo->id;
    if (closedSet[u])
      continue;
    closedSet[u] = true;

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
      if (closedSet[v])
        continue;

      float tentativeG = currentInfo->g + edge.weight;
      if (tentativeG < info[v].g) {
        info[v].parentId = u;
        graph.nodes[v].parentId = u;
        info[v].g = tentativeG;
        info[v].h = 0;
        openSet.push(&info[v]);

        if (graph.nodes[v].state == NodeState::NORMAL) {
          graph.nodes[v].state = NodeState::OPEN;
          frames.push_back(makeFrame(graph));
        }
      }
    }
  }

  if (found) {
    appendPathFrames(graph, info[graph.endId].parentId, info, frames);
  }
  return frames;
}
