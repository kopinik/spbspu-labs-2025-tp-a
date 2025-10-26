#include <limits>
#include <iterator>
#include <vector>
#include <algorithm>

#include "data_struct.hpp"

int main()
{
  using namespace fedorov;
  using input_it_t = std::istream_iterator< DataStruct >;
  using output_it_t = std::ostream_iterator< DataStruct >;

  std::vector< DataStruct > data(input_it_t{std::cin}, input_it_t{});

  while (!std::cin.eof())
  {
    if (std::cin.fail())
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    std::copy(input_it_t{std::cin}, input_it_t{}, std::back_inserter(data));
  }
  std::sort(data.begin(), data.end());
  std::copy(data.cbegin(), data.cend(), output_it_t{std::cout, "\n"});
}
