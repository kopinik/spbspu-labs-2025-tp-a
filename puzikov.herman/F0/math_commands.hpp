#ifndef MATH_COMMANDS_HPP
#define MATH_COMMANDS_HPP

#include <iostream>
#include <unordered_map>
#include "matrix.hpp"

namespace puzikov
{
  using matrices_map = std::unordered_map< std::string, puzikov::Matrix >;

  void addCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void subCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void mulCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void mulScalarCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void transposeCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void hstackCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void vstackCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void flattenRowCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void flattenColCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
}

#endif
