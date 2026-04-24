#include "Pathfinder.h"
#include <cmath>
#include <queue>
#include <vector>

struct NodeInfo {
  int id;
  float g, h;
  int parentId;

  NodeInfo() : id(-1), g(1e9), h(0), parentId(-1) {}
  float f() const { return g + h; }
};

struct CompareF {
  bool operator()(const NodeInfo *a, const NodeInfo *b) {
    return a->f() > b->f();
  }
};

static float get_distance(const Node &a, const Node &b) {
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

void Pathfinder::resetGraph(GraphData &graph) {
  for (auto &node : graph.nodes) {
    node.parentId = -1;
    if (node.state != NodeState::START && node.state != NodeState::END) {
      node.state = NodeState::NORMAL;
    }
  }
}

void AStarPathfinder::solve(GraphData &graph) {
  if (graph.startId == -1 || graph.endId == -1)
    return;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  for (int i = 0; i < n; ++i)
    info[i].id = i;

  std::priority_queue<NodeInfo *, std::vector<NodeInfo *>, CompareF> openSet;
  std::vector<bool> closedSet(n, false);

  NodeInfo &start = info[graph.startId];
  start.g = 0;
  start.h = get_distance(graph.nodes[graph.startId], graph.nodes[graph.endId]);
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
    }

    if (u == graph.endId) {
      found = true;
      break;
    }

    for (const auto &edge : graph.adj[u]) {
      int v = edge.targetId;
      if (closedSet[v])
        continue;

      float tentative_g = currentInfo->g + edge.weight;
      if (tentative_g < info[v].g) {
        info[v].parentId = u;
        graph.nodes[v].parentId = u;
        info[v].g = tentative_g;
        info[v].h = get_distance(graph.nodes[v], graph.nodes[graph.endId]);
        openSet.push(&info[v]);

        if (graph.nodes[v].state == NodeState::NORMAL) {
          graph.nodes[v].state = NodeState::OPEN;
        }
      }
    }
  }

  if (found) {
    int curr = info[graph.endId].parentId;
    while (curr != -1 && curr != graph.startId) {
      graph.nodes[curr].state = NodeState::PATH;
      curr = info[curr].parentId;
    }
  }
}

void DijkstraPathfinder::solve(GraphData &graph) {
  if (graph.startId == -1 || graph.endId == -1)
    return;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  for (int i = 0; i < n; ++i)
    info[i].id = i;

  std::priority_queue<NodeInfo *, std::vector<NodeInfo *>, CompareF> openSet;
  std::vector<bool> closedSet(n, false);

  NodeInfo &start = info[graph.startId];
  start.g = 0;
  start.h = 0; // Dijkstra no usa heurística (h=0)
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
    }

    if (u == graph.endId) {
      found = true;
      break;
    }

    for (const auto &edge : graph.adj[u]) {
      int v = edge.targetId;
      if (closedSet[v])
        continue;

      float tentative_g = currentInfo->g + edge.weight;
      if (tentative_g < info[v].g) {
        info[v].parentId = u;
        graph.nodes[v].parentId = u;
        info[v].g = tentative_g;
        info[v].h = 0; // h es 0 para Dijkstra
        openSet.push(&info[v]);

        if (graph.nodes[v].state == NodeState::NORMAL) {
          graph.nodes[v].state = NodeState::OPEN;
        }
      }
    }
  }

  if (found) {
    int curr = info[graph.endId].parentId;
    while (curr != -1 && curr != graph.startId) {
      graph.nodes[curr].state = NodeState::PATH;
      curr = info[curr].parentId;
    }
  }
}
