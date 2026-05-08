#include "Visualizer.h"
#include <GL/gl.h>
#include <cctype>

namespace {
void drawRect(float x, float y, float width, float height, float r, float g,
              float b) {
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y - height);
  glVertex2f(x, y - height);
  glEnd();
}

void drawRectOutline(float x, float y, float width, float height, float r,
                     float g, float b) {
  glColor3f(r, g, b);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y - height);
  glVertex2f(x, y - height);
  glEnd();
}

const char **glyphFor(char value) {
  static const char *space[] = {"00000", "00000", "00000", "00000",
                                "00000", "00000", "00000"};
  static const char *a[] = {"01110", "10001", "10001", "11111",
                            "10001", "10001", "10001"};
  static const char *b[] = {"11110", "10001", "10001", "11110",
                            "10001", "10001", "11110"};
  static const char *letterC[] = {"01111", "10000", "10000", "10000",
                                  "10000", "10000", "01111"};
  static const char *d[] = {"11110", "10001", "10001", "10001",
                            "10001", "10001", "11110"};
  static const char *e[] = {"11111", "10000", "10000", "11110",
                            "10000", "10000", "11111"};
  static const char *f[] = {"11111", "10000", "10000", "11110",
                            "10000", "10000", "10000"};
  static const char *g[] = {"01111", "10000", "10000", "10011",
                            "10001", "10001", "01110"};
  static const char *i[] = {"11111", "00100", "00100", "00100",
                            "00100", "00100", "11111"};
  static const char *j[] = {"00111", "00010", "00010", "00010",
                            "10010", "10010", "01100"};
  static const char *k[] = {"10001", "10010", "10100", "11000",
                            "10100", "10010", "10001"};
  static const char *l[] = {"10000", "10000", "10000", "10000",
                            "10000", "10000", "11111"};
  static const char *m[] = {"10001", "11011", "10101", "10101",
                            "10001", "10001", "10001"};
  static const char *n[] = {"10001", "11001", "10101", "10011",
                            "10001", "10001", "10001"};
  static const char *o[] = {"01110", "10001", "10001", "10001",
                            "10001", "10001", "01110"};
  static const char *p[] = {"11110", "10001", "10001", "11110",
                            "10000", "10000", "10000"};
  static const char *q[] = {"01110", "10001", "10001", "10001",
                            "10101", "10010", "01101"};
  static const char *r[] = {"11110", "10001", "10001", "11110",
                            "10100", "10010", "10001"};
  static const char *s[] = {"01111", "10000", "10000", "01110",
                            "00001", "00001", "11110"};
  static const char *t[] = {"11111", "00100", "00100", "00100",
                            "00100", "00100", "00100"};
  static const char *u[] = {"10001", "10001", "10001", "10001",
                            "10001", "10001", "01110"};
  static const char *v[] = {"10001", "10001", "10001", "10001",
                            "01010", "01010", "00100"};
  static const char *z[] = {"11111", "00001", "00010", "00100",
                            "01000", "10000", "11111"};
  static const char *zero[] = {"01110", "10001", "10011", "10101",
                               "11001", "10001", "01110"};
  static const char *one[] = {"00100", "01100", "00100", "00100",
                              "00100", "00100", "01110"};
  static const char *two[] = {"01110", "10001", "00001", "00010",
                              "00100", "01000", "11111"};
  static const char *three[] = {"11110", "00001", "00001", "01110",
                                "00001", "00001", "11110"};
  static const char *four[] = {"10010", "10010", "10010", "11111",
                               "00010", "00010", "00010"};
  static const char *star[] = {"00000", "10101", "01110", "11111",
                               "01110", "10101", "00000"};

  switch (std::toupper(static_cast<unsigned char>(value))) {
  case 'A':
    return a;
  case 'B':
    return b;
  case 'C':
    return letterC;
  case 'D':
    return d;
  case 'E':
    return e;
  case 'F':
    return f;
  case 'G':
    return g;
  case 'I':
    return i;
  case 'J':
    return j;
  case 'K':
    return k;
  case 'L':
    return l;
  case 'M':
    return m;
  case 'N':
    return n;
  case 'O':
    return o;
  case 'P':
    return p;
  case 'Q':
    return q;
  case 'R':
    return r;
  case 'S':
    return s;
  case 'T':
    return t;
  case 'U':
    return u;
  case 'V':
    return v;
  case 'Z':
    return z;
  case '0':
    return zero;
  case '1':
    return one;
  case '2':
    return two;
  case '3':
    return three;
  case '4':
    return four;
  case '*':
    return star;
  default:
    return space;
  }
}

void drawText(const std::string &text, float x, float y, float scale, float r,
              float g, float b) {
  const float pixel = scale;
  const float gap = scale * 0.75f;
  float cursor = x;

  glColor3f(r, g, b);
  glLineWidth(1.4f);
  for (char c : text) {
    const char **glyph = glyphFor(c);
    glBegin(GL_LINES);
    for (int row = 0; row < 7; ++row) {
      for (int col = 0; col < 5; ++col) {
        if (glyph[row][col] != '1')
          continue;

        float cx = cursor + (col + 0.5f) * pixel;
        float cy = y - (row + 0.5f) * pixel;

        if (col + 1 < 5 && glyph[row][col + 1] == '1') {
          glVertex2f(cx, cy);
          glVertex2f(cursor + (col + 1.5f) * pixel, cy);
        }
        if (row + 1 < 7 && glyph[row + 1][col] == '1') {
          glVertex2f(cx, cy);
          glVertex2f(cx, y - (row + 1.5f) * pixel);
        }
        bool isolatedRight = col + 1 >= 5 || glyph[row][col + 1] != '1';
        bool isolatedDown = row + 1 >= 7 || glyph[row + 1][col] != '1';
        if (isolatedRight && isolatedDown) {
          glVertex2f(cx - pixel * 0.18f, cy);
          glVertex2f(cx + pixel * 0.18f, cy);
        }
      }
    }
    glEnd();
    cursor += 5 * pixel + gap;
  }
  glLineWidth(1.0f);
}

void drawMenu(const std::vector<MenuItem> &menu) {
  // Panel de fondo
  drawRect(0.59f, 0.90f, 0.40f, 1.10f, 0.11f, 0.12f, 0.14f);
  drawRectOutline(0.59f, 0.90f, 0.40f, 1.10f, 0.18f, 0.19f, 0.22f);

  // Header
  drawRect(0.60f, 0.89f, 0.39f, 0.055f, 0.13f, 0.14f, 0.16f);
  drawRectOutline(0.60f, 0.89f, 0.39f, 0.055f, 0.18f, 0.19f, 0.22f);

  drawText("MENU", 0.725f, 0.882f, 0.0052f, 0.60f, 0.63f, 0.67f);

  // Línea divisora bajo el título
  glColor3f(0.17f, 0.18f, 0.21f);
  glBegin(GL_LINES);
  glVertex2f(0.62f, 0.832f);
  glVertex2f(0.93f, 0.832f);
  glEnd();

  for (const auto &item : menu) {
    if (item.selected) {
      drawRect(item.x, item.y, item.width, item.height, 0.10f, 0.23f, 0.36f);
      drawRectOutline(item.x, item.y, item.width, item.height, 0.18f, 0.42f,
                      0.67f);
    } else {
      drawRect(item.x, item.y, item.width, item.height, 0.12f, 0.13f, 0.15f);
      drawRectOutline(item.x, item.y, item.width, item.height, 0.17f, 0.18f,
                      0.21f);
    }

    const float scale      = 0.0048f;
    const float glyphH     = 7.0f * scale;
    const float textY      = item.y - (item.height - glyphH) * 0.5f;
    
    drawText(item.shortcut,
             item.x + 0.012f,
             textY,
             scale,
             0.29f, 0.42f, 0.65f);

    const float charW      = (5.0f + 0.75f) * scale;
    const float shortcutW  = item.shortcut.size() * charW;
    drawText(item.label,
             item.x + 0.012f + shortcutW + 0.010f,
             textY,
             scale,
             0.86f, 0.88f, 0.91f);
  }
}
} // namespace

void Visualizer::draw(const GraphData &graph,
                      const std::vector<MenuItem> &menu, int selectedNodeId) {
  // Dibujar conexiones
  glLineWidth(2.0f);
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
  glLineWidth(1.0f);

  // Dibujar nodos
  glPointSize(10.5f);
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

  if (selectedNodeId >= 0 && selectedNodeId < (int)graph.nodes.size()) {
    const Node &node = graph.nodes[selectedNodeId];
    glPointSize(17.0f);
    glColor3f(1.0f, 0.95f, 0.35f);
    glBegin(GL_POINTS);
    glVertex2f(node.x, node.y);
    glEnd();

    glPointSize(9.0f);
    glColor3f(0.10f, 0.10f, 0.10f);
    glBegin(GL_POINTS);
    glVertex2f(node.x, node.y);
    glEnd();
  }

  drawMenu(menu);
}
