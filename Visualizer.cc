#include "Visualizer.h"
#include <GL/gl.h>
#include <cstddef>

void Visualizer::draw(const GraphData &graph) {
  // 1. Dibujar conexiones (Aristas)
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
    const Node &u = graph.nodes[i];
    for (const auto &edge : graph.adj[i]) {
      const Node &v = graph.nodes[edge.targetId];

      // Si ambos nodos son del camino, no dibujar aquí para evitar solapamiento
      if (u.state == NodeState::PATH && v.state == NodeState::PATH) {
        continue;
      } else {
        glColor3f(0.3f, 0.3f, 0.3f);
        glVertex2f(u.x, u.y);
        glVertex2f(v.x, v.y);
      }
    }
  }
  glEnd();

  // Dibujar camino destacado
  glLineWidth(3.0f);
  glColor3f(1.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
  for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
    const Node &u = graph.nodes[i];
    if (u.state != NodeState::PATH && u.state != NodeState::START &&
        u.state != NodeState::END)
      continue;
    for (const auto &edge : graph.adj[i]) {
      const Node &v = graph.nodes[edge.targetId];
      if ((u.state == NodeState::PATH || u.state == NodeState::START) &&
          (v.state == NodeState::PATH || v.state == NodeState::END)) {
        glVertex2f(u.x, u.y);
        glVertex2f(v.x, v.y);
      }
    }
  }
  glEnd();

  // 2. Dibujar Nodos
  float size = 0.02f;
  for (const auto &node : graph.nodes) {
    switch (node.state) {
    case NodeState::NORMAL:
      glColor3f(0.5f, 0.5f, 0.5f);
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
      glColor3f(0.0f, 0.5f, 0.0f);
      break;
    case NodeState::CLOSED:
      glColor3f(0.3f, 0.3f, 0.6f);
      break;
    }

    glBegin(GL_QUADS);
    glVertex2f(node.x - size, node.y - size);
    glVertex2f(node.x + size, node.y - size);
    glVertex2f(node.x + size, node.y + size);
    glVertex2f(node.x - size, node.y + size);
    glEnd();
  }
}
