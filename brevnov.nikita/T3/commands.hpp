#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "polygon.hpp"
#include <iosfwd>
#include <vector>
namespace brevnov
{
  void area(std::istream&, std::ostream&, const std::vector< Polygon >&);
  void max(std::istream&, std::ostream&, const std::vector< Polygon >&);
  void min(std::istream&, std::ostream&, const std::vector< Polygon >&);
  void count(std::istream&, std::ostream&, const std::vector< Polygon >&);
  void intersections(std::istream&, std::ostream&, const std::vector< Polygon >&);
  void rightshapes(std::istream&, std::ostream&, const std::vector< Polygon >&);
}
#endif
