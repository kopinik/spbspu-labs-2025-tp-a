#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <limits>
#include "utility_commands.hpp"
#include "math_commands.hpp"

using namespace puzikov;

int main()
{
  matrices_map matrices;
  std::unordered_map< std::string, std::function< void(std::istream &, std::ostream &) > > cmds;

  {
    using std::placeholders::_1;
    using std::placeholders::_2;
    cmds["create"] = std::bind(createMatrCommand, _1, _2, std::ref(matrices));
    cmds["create_filled"] = std::bind(createFilledMatrCommand, _1, _2, std::ref(matrices));
    cmds["matrix_list"] = std::bind(listMatrCommand, _2, std::cref(matrices));
    cmds["size"] = std::bind(sizeCommand, _1, _2, std::cref(matrices));
    cmds["print"] = std::bind(printCommand, _1, _2, std::cref(matrices));
    cmds["drop"] = std::bind(dropMatrCommand, _1, _2, std::ref(matrices));
    cmds["set"] = std::bind(setCommand, _1, _2, std::ref(matrices));
    cmds["setrow"] = std::bind(setrowCommand, _1, _2, std::ref(matrices));
    cmds["setcol"] = std::bind(setcolCommand, _1, _2, std::ref(matrices));
    cmds["add"] = std::bind(addCommand, _1, _2, std::ref(matrices));
    cmds["sub"] = std::bind(subCommand, _1, _2, std::ref(matrices));
    cmds["mul"] = std::bind(mulCommand, _1, _2, std::ref(matrices));
    cmds["mul_scalar"] = std::bind(mulScalarCommand, _1, _2, std::ref(matrices));
    cmds["transpose"] = std::bind(transposeCommand, _1, _2, std::ref(matrices));
    cmds["hstack"] = std::bind(hstackCommand, _1, _2, std::ref(matrices));
    cmds["vstack"] = std::bind(vstackCommand, _1, _2, std::ref(matrices));
    cmds["flatten_row"] = std::bind(flattenRowCommand, _1, _2, std::ref(matrices));
    cmds["flatten_col"] = std::bind(flattenColCommand, _1, _2, std::ref(matrices));
    cmds["set_precision"] = std::bind(setPrecisionCommand, _1, _2);
    cmds["set_epsilon"] = std::bind(setEpsilonCommand, _1, _2);
  }

  std::string command;
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout.precision(4);

  while (!(std::cin >> command).eof())
  {
    try
    {
      cmds.at(command)(std::cin, std::cout);
      std::cout << '\n';
    }
    catch (std::exception &)
    {
      if (std::cin.fail())
      {
        std::cin.clear(std::ios::goodbit);
      }
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  return 0;
}
