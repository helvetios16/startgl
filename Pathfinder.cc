#include "Pathfinder.h"
#include <cmath>
#include <deque>
#include <iostream>
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

static SearchFrame make_frame(const GraphData &graph) {
  SearchFrame frame;
  frame.states.reserve(graph.nodes.size());
  frame.parents.reserve(graph.nodes.size());
  for (const auto &node : graph.nodes) {
    frame.states.push_back(node.state);
    frame.parents.push_back(node.parentId);
  }
  return frame;
}

static void apply_frame(GraphData &graph, const SearchFrame &frame) {
  for (int i = 0; i < (int)graph.nodes.size(); ++i) {
    graph.nodes[i].state = frame.states[i];
    graph.nodes[i].parentId = frame.parents[i];
  }
}

static void mark_path(GraphData &graph, int endParent,
                      std::vector<NodeInfo> &info,
                      std::vector<SearchFrame> &frames) {
  int curr = endParent;
  while (curr != -1 && curr != graph.startId) {
    graph.nodes[curr].state = NodeState::PATH;
    frames.push_back(make_frame(graph));
    curr = info[curr].parentId;
  }
}

static std::vector<SearchFrame> solve_weighted_steps(const GraphData &source,
                                                     bool useHeuristic) {
  GraphData graph = source;
  Pathfinder::resetGraph(graph);
  std::vector<SearchFrame> frames;
  frames.push_back(make_frame(graph));

  if (graph.startId == -1 || graph.endId == -1)
    return frames;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  for (int i = 0; i < n; ++i)
    info[i].id = i;

  std::priority_queue<NodeInfo *, std::vector<NodeInfo *>, CompareF> openSet;
  std::vector<bool> closedSet(n, false);

  NodeInfo &start = info[graph.startId];
  start.g = 0;
  start.h =
      useHeuristic ? get_distance(graph.nodes[graph.startId],
                                  graph.nodes[graph.endId])
                   : 0;
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
      frames.push_back(make_frame(graph));
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
        info[v].h = useHeuristic
                        ? get_distance(graph.nodes[v], graph.nodes[graph.endId])
                        : 0;
        openSet.push(&info[v]);

        if (graph.nodes[v].state == NodeState::NORMAL) {
          graph.nodes[v].state = NodeState::OPEN;
          frames.push_back(make_frame(graph));
        }
      }
    }
  }

  if (found) {
    mark_path(graph, info[graph.endId].parentId, info, frames);
  }
  return frames;
}

static std::vector<SearchFrame> solve_unweighted_steps(const GraphData &source,
                                                       bool depthFirst) {
  GraphData graph = source;
  Pathfinder::resetGraph(graph);
  std::vector<SearchFrame> frames;
  frames.push_back(make_frame(graph));

  if (graph.startId == -1 || graph.endId == -1)
    return frames;

  int n = (int)graph.nodes.size();
  std::vector<NodeInfo> info(n);
  std::vector<bool> visited(n, false);
  for (int i = 0; i < n; ++i)
    info[i].id = i;

  std::deque<int> frontier;
  frontier.push_back(graph.startId);
  visited[graph.startId] = true;

  bool found = false;
  while (!frontier.empty()) {
    int u;
    if (depthFirst) {
      u = frontier.back();
      frontier.pop_back();
    } else {
      u = frontier.front();
      frontier.pop_front();
    }

    if (graph.nodes[u].state == NodeState::NORMAL) {
      graph.nodes[u].state = NodeState::CLOSED;
      frames.push_back(make_frame(graph));
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
      frontier.push_back(v);

      if (graph.nodes[v].state == NodeState::NORMAL) {
        graph.nodes[v].state = NodeState::OPEN;
        frames.push_back(make_frame(graph));
      }
    }
  }

  if (found) {
    mark_path(graph, info[graph.endId].parentId, info, frames);
  }
  return frames;
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
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    apply_frame(graph, frames.back());
}

void DijkstraPathfinder::solve(GraphData &graph) {
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    apply_frame(graph, frames.back());
}

void BFSPathfinder::solve(GraphData &graph) {
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    apply_frame(graph, frames.back());
}

void DFSPathfinder::solve(GraphData &graph) {
  std::vector<SearchFrame> frames = solveSteps(graph);
  if (!frames.empty())
    apply_frame(graph, frames.back());
}

std::vector<SearchFrame> AStarPathfinder::solveSteps(const GraphData &graph) {
  return solve_weighted_steps(graph, true);
}

std::vector<SearchFrame> DijkstraPathfinder::solveSteps(const GraphData &graph) {
  return solve_weighted_steps(graph, false);
}

std::vector<SearchFrame> BFSPathfinder::solveSteps(const GraphData &graph) {
  return solve_unweighted_steps(graph, false);
}

std::vector<SearchFrame> DFSPathfinder::solveSteps(const GraphData &graph) {
  return solve_unweighted_steps(graph, true);
}
