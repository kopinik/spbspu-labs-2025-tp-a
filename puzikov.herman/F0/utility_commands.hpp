#ifndef UTILITY_COMMANDS_HPP
#define UTILITY_COMMANDS_HPP

#include <iostream>
#include <unordered_map>
#include "matrix.hpp"

namespace puzikov
{
  using matrices_map = std::unordered_map< std::string, puzikov::Matrix >;

  void createMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void createFilledMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void listMatrCommand(std::ostream &out, const matrices_map &matrices);
  void sizeCommand(std::istream &in, std::ostream &out, const matrices_map &matrices);
  void printCommand(std::istream &in, std::ostream &out, const matrices_map &matrices);
  void dropMatrCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void setCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void setrowCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void setcolCommand(std::istream &in, std::ostream &out, matrices_map &matrices);
  void setPrecisionCommand(std::istream &in, std::ostream &out);
  void setEpsilonCommand(std::istream &in, std::ostream &out);
}

#endif
