#include "Visualizer.h"
#include <GL/gl.h>

void Visualizer::draw(const GraphData &graph) {
  // Dibujar conexiones
  glBegin(GL_LINES);
  for (int i = 0; i < (int)graph.adj.size(); ++i) {
    for (const auto &edge : graph.adj[i]) {
      int u = i;
      int v = edge.targetId;

      // Por defecto gris oscuro
      glColor3f(0.2f, 0.2f, 0.2f);

      // Si hay una relación padre-hijo, pintamos la arista
      if (graph.nodes[v].parentId == u || graph.nodes[u].parentId == v) {
        int childId = (graph.nodes[v].parentId == u) ? v : u;
        NodeState childState = graph.nodes[childId].state;

        switch (childState) {
        case NodeState::PATH:
        case NodeState::END:
          glColor3f(1.0f, 1.0f, 0.0f); // Camino (Amarillo)
          break;
        case NodeState::CLOSED:
          glColor3f(0.5f, 0.0f, 0.5f); // Explorado (Púrpura)
          break;
        case NodeState::OPEN:
          glColor3f(0.0f, 0.5f, 1.0f); // Frontera (Azul claro)
          break;
        default:
          glColor3f(0.3f, 0.3f, 0.3f);
          break;
        }
      }

      glVertex2f(graph.nodes[u].x, graph.nodes[u].y);
      glVertex2f(graph.nodes[v].x, graph.nodes[v].y);
    }
  }
  glEnd();

  // Dibujar nodos
  glPointSize(8.0f);
  glBegin(GL_POINTS);
  for (const auto &node : graph.nodes) {
    switch (node.state) {
    case NodeState::NORMAL:
      glColor3f(0.7f, 0.7f, 0.7f);
      break;
    case NodeState::START:
      glColor3f(0.0f, 1.0f, 0.0f);
      break;
    case NodeState::END:
      glColor3f(1.0f, 0.0f, 0.0f);
      break;
    case NodeState::PATH:
      glColor3f(1.0f, 1.0f, 0.0f);
      break;
    case NodeState::OPEN:
      glColor3f(0.0f, 0.5f, 1.0f);
      break;
    case NodeState::CLOSED:
      glColor3f(0.5f, 0.0f, 0.5f);
      break;
    }
    glVertex2f(node.x, node.y);
  }
  glEnd();
}
