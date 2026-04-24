#include "Visualizer.h"
#include <GL/gl.h>
#include <cmath>

void Visualizer::draw(const GraphData &graph) {
  // Dibujar conexiones
  glColor3f(0.3f, 0.3f, 0.3f);
  glBegin(GL_LINES);
  for (int i = 0; i < (int)graph.adj.size(); ++i) {
    for (const auto &edge : graph.adj[i]) {
      glVertex2f(graph.nodes[i].x, graph.nodes[i].y);
      glVertex2f(graph.nodes[edge.targetId].x, graph.nodes[edge.targetId].y);
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
