#pragma once
#include "Graph.h"
#include <string>
#include <vector>

struct MenuItem {
  MenuItem(const std::string &id, const std::string &label,
           const std::string &shortcut, float x, float y, float width,
           float height, bool selected)
      : id(id), label(label), shortcut(shortcut), x(x), y(y), width(width),
        height(height), selected(selected) {}

  std::string id;
  std::string label;
  std::string shortcut;
  float x;
  float y;
  float width;
  float height;
  bool selected;
};

class Visualizer {
public:
  static void draw(const GraphData &graph, const std::vector<MenuItem> &menu);
};
