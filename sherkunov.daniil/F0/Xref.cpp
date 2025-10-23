#include "Xref.h"

void Xref::add(const std::string &word, std::size_t line)
{
  if (word.empty()) {
    return;
  }

  auto &lines = map_[word];
  if (lines.empty() || lines.back() != line) {
    lines.push_back(line);
  }
}

const std::map<std::string, std::vector<std::size_t>> &Xref::data() const noexcept
{
  return map_;
}
