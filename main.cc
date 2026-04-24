#include "Graph.h"
#include "Pathfinder.h"
#include "Visualizer.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

int main() {
  std::srand(std::time(nullptr));

  if (!glfwInit())
    return -1;
  GLFWwindow *window = glfwCreateWindow(
      800, 800, "OpenGL Node Pathfinding Visualization", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  GraphData graph;
  int numNodes = 50;

  // 1. Generar nodos aleatorios
  for (int i = 0; i < numNodes; ++i) {
    Node n;
    n.id = i;
    n.x = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
    n.y = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
    graph.nodes.push_back(n);
  }

  graph.adj.resize(numNodes);

  // 2. Conectar nodos si están cerca
  float maxDist = 0.4f;
  for (int i = 0; i < numNodes; ++i) {
    for (int j = i + 1; j < numNodes; ++j) {
      float d = std::sqrt(std::pow(graph.nodes[i].x - graph.nodes[j].x, 2) +
                          std::pow(graph.nodes[i].y - graph.nodes[j].y, 2));
      if (d < maxDist) {
        graph.adj[i].push_back({j, d});
        graph.adj[j].push_back({i, d});
      }
    }
  }

  // 3. Definir inicio y fin
  graph.startId = 0;
  graph.endId = numNodes - 1;
  graph.nodes[graph.startId].state = NodeState::START;
  graph.nodes[graph.endId].state = NodeState::END;

  // 4. Resolver
  Pathfinder::solve(graph);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Visualizer::draw(graph);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
