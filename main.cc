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
std::vector<MenuItem> menuItems;

const char *algoName(AlgoType type) {
  switch (type) {
  case AlgoType::ASTAR:
    return "A*";
  case AlgoType::DIJKSTRA:
    return "Dijkstra";
  case AlgoType::BFS:
    return "BFS";
  case AlgoType::DFS:
    return "DFS";
  }
  return "A*";
}

void selectAlgorithm(AlgoType type) {
  selectedType = type;
  switch (type) {
  case AlgoType::ASTAR:
    currentAlgo = std::unique_ptr<AStarPathfinder>(new AStarPathfinder());
    break;
  case AlgoType::DIJKSTRA:
    currentAlgo = std::unique_ptr<DijkstraPathfinder>(new DijkstraPathfinder());
    break;
  case AlgoType::BFS:
    currentAlgo = std::unique_ptr<BFSPathfinder>(new BFSPathfinder());
    break;
  case AlgoType::DFS:
    currentAlgo = std::unique_ptr<DFSPathfinder>(new DFSPathfinder());
    break;
  }
  std::cout << "Algoritmo seleccionado: " << algoName(type) << std::endl;
}

void solveGraph() {
  if (!currentAlgo) {
    selectAlgorithm(selectedType);
  }
  Pathfinder::resetGraph(globalGraph);
  std::cout << "Resolviendo con " << algoName(selectedType) << "..."
            << std::endl;
  currentAlgo->solve(globalGraph);
}

std::vector<MenuItem> buildMenu() {
  const float x = 0.63f;
  const float y = 0.72f;
  const float width = 0.28f;
  const float height = 0.07f;
  const float gap = 0.025f;

  std::vector<MenuItem> items = {
      {"astar", "A*", "1", x, y, width, height,
       selectedType == AlgoType::ASTAR},
      {"dijkstra", "DIJKSTRA", "2", x, y - (height + gap), width, height,
       selectedType == AlgoType::DIJKSTRA},
      {"bfs", "BFS", "3", x, y - 2 * (height + gap), width, height,
       selectedType == AlgoType::BFS},
      {"dfs", "DFS", "4", x, y - 3 * (height + gap), width, height,
       selectedType == AlgoType::DFS},
      {"new", "NUEVO", "N", x, y - 4 * (height + gap), width, height, false},
      {"clear", "LIMPIAR", "C", x, y - 5 * (height + gap), width, height,
       false},
      {"solve", "RESOLVER", "SP", x, y - 6 * (height + gap), width, height,
       false},
      {"quit", "SALIR", "Q", x, y - 7 * (height + gap), width, height, false},
  };
  return items;
}

void refreshMenu() { menuItems = buildMenu(); }

void generateNewGraph() {
  globalGraph.nodes.clear();
  globalGraph.adj.clear();

  int numNodes = 60;
  for (int i = 0; i < numNodes; ++i) {
    Node n;
    n.id = i;
    n.x = ((float)rand() / RAND_MAX) * 1.42f - 0.9f;
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

void handleMenuAction(GLFWwindow *window, const std::string &id) {
  if (id == "astar") {
    selectAlgorithm(AlgoType::ASTAR);
  } else if (id == "dijkstra") {
    selectAlgorithm(AlgoType::DIJKSTRA);
  } else if (id == "bfs") {
    selectAlgorithm(AlgoType::BFS);
  } else if (id == "dfs") {
    selectAlgorithm(AlgoType::DFS);
  } else if (id == "new") {
    generateNewGraph();
  } else if (id == "clear") {
    Pathfinder::resetGraph(globalGraph);
    std::cout << "Grafo limpiado (manteniendo estructura)." << std::endl;
  } else if (id == "solve") {
    solveGraph();
  } else if (id == "quit") {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  refreshMenu();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action != GLFW_PRESS)
    return;

  if (key == GLFW_KEY_1) {
    selectAlgorithm(AlgoType::ASTAR);
  } else if (key == GLFW_KEY_2) {
    selectAlgorithm(AlgoType::DIJKSTRA);
  } else if (key == GLFW_KEY_3) {
    selectAlgorithm(AlgoType::BFS);
  } else if (key == GLFW_KEY_4) {
    selectAlgorithm(AlgoType::DFS);
  } else if (key == GLFW_KEY_N) {
    generateNewGraph();
  } else if (key == GLFW_KEY_C) {
    Pathfinder::resetGraph(globalGraph);
    std::cout << "Grafo limpiado (manteniendo estructura)." << std::endl;
  } else if (key == GLFW_KEY_Q) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_SPACE) {
    solveGraph();
  }
  refreshMenu();
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
    return;

  double cursorX;
  double cursorY;
  int width;
  int height;
  glfwGetCursorPos(window, &cursorX, &cursorY);
  glfwGetWindowSize(window, &width, &height);

  float x = static_cast<float>((cursorX / width) * 2.0 - 1.0);
  float y = static_cast<float>(1.0 - (cursorY / height) * 2.0);

  for (const auto &item : menuItems) {
    bool insideX = x >= item.x && x <= item.x + item.width;
    bool insideY = y <= item.y && y >= item.y - item.height;
    if (insideX && insideY) {
      handleMenuAction(window, item.id);
      break;
    }
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
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  generateNewGraph();
  currentAlgo = std::unique_ptr<AStarPathfinder>(new AStarPathfinder());
  refreshMenu();

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

    Visualizer::draw(globalGraph, menuItems);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
