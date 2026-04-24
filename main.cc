#include "Graph.h"
#include "Pathfinder.h"
#include "Visualizer.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>

GraphData globalGraph;
std::unique_ptr<IPathfinder> currentAlgo;
enum class AlgoType { ASTAR, DIJKSTRA, BFS, DFS };
AlgoType selectedType = AlgoType::ASTAR;

void generateNewGraph() {
  globalGraph.nodes.clear();
  globalGraph.adj.clear();

  int numNodes = 60;
  for (int i = 0; i < numNodes; ++i) {
    Node n;
    n.id = i;
    n.x = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
    n.y = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
    globalGraph.nodes.push_back(n);
  }

  globalGraph.adj.resize(numNodes);
  float maxDist = 0.35f;
  for (int i = 0; i < numNodes; ++i) {
    for (int j = i + 1; j < numNodes; ++j) {
      float d = std::sqrt(
          std::pow(globalGraph.nodes[i].x - globalGraph.nodes[j].x, 2) +
          std::pow(globalGraph.nodes[i].y - globalGraph.nodes[j].y, 2));
      if (d < maxDist) {
        globalGraph.adj[i].push_back({j, d});
        globalGraph.adj[j].push_back({i, d});
      }
    }
  }

  globalGraph.startId = 0;
  globalGraph.endId = numNodes - 1;
  globalGraph.nodes[globalGraph.startId].state = NodeState::START;
  globalGraph.nodes[globalGraph.endId].state = NodeState::END;
  std::cout << "Nuevo grafo generado." << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action != GLFW_PRESS)
    return;

  if (key == GLFW_KEY_1) {
    selectedType = AlgoType::ASTAR;
    currentAlgo = std::unique_ptr<AStarPathfinder>(new AStarPathfinder());
    std::cout << "Algoritmo seleccionado: A*" << std::endl;
  } else if (key == GLFW_KEY_2) {
    selectedType = AlgoType::DIJKSTRA;
    currentAlgo = std::unique_ptr<DijkstraPathfinder>(new DijkstraPathfinder());
    std::cout << "Algoritmo seleccionado: Dijkstra" << std::endl;
  } else if (key == GLFW_KEY_3) {
    selectedType = AlgoType::BFS;
    currentAlgo = std::unique_ptr<BFSPathfinder>(new BFSPathfinder());
    std::cout << "Algoritmo seleccionado: BFS" << std::endl;
  } else if (key == GLFW_KEY_4) {
    selectedType = AlgoType::DFS;
    currentAlgo = std::unique_ptr<DFSPathfinder>(new DFSPathfinder());
    std::cout << "Algoritmo seleccionado: DFS" << std::endl;
  } else if (key == GLFW_KEY_N) {
    generateNewGraph();
  } else if (key == GLFW_KEY_C) {
    Pathfinder::resetGraph(globalGraph);
    std::cout << "Grafo limpiado (manteniendo estructura)." << std::endl;
  } else if (key == GLFW_KEY_Q) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_SPACE) {
    if (!currentAlgo) {
      currentAlgo = std::unique_ptr<AStarPathfinder>(new AStarPathfinder());
    }
    Pathfinder::resetGraph(globalGraph);
    std::cout << "Resolviendo con "
              << (selectedType == AlgoType::ASTAR ? "A*" : "Dijkstra") << "..."
              << std::endl;
    currentAlgo->solve(globalGraph);
  }
}

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
  glfwSetKeyCallback(window, key_callback);

  generateNewGraph();
  currentAlgo = std::unique_ptr<AStarPathfinder>(new AStarPathfinder());

  std::cout << "--- Controles ---" << std::endl;
  std::cout << "[1] Seleccionar A*" << std::endl;
  std::cout << "[2] Seleccionar Dijkstra" << std::endl;
  std::cout << "[3] Seleccionar BFS" << std::endl;
  std::cout << "[4] Seleccionar DFS" << std::endl;
  std::cout << "[N] Generar nuevo grafo" << std::endl;
  std::cout << "[C] Limpiar solución actual" << std::endl;
  std::cout << "[SPACE] Resolver camino" << std::endl;
  std::cout << "[Q] Salir" << std::endl;

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Visualizer::draw(globalGraph);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
