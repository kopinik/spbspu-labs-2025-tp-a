#ifndef SHERKUNOV_DANIIL_F0_XREF_H
#define SHERKUNOV_DANIIL_F0_XREF_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

class Xref
{
public:
  Xref() = default;
  void add(const std::string &word, std::size_t line);
  const std::map<std::string, std::vector<std::size_t>> &data() const noexcept;

private:
  std::map<std::string, std::vector<std::size_t>> map_;
};

#endif
