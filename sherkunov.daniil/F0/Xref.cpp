#include "Xref.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <iterator>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace sherkunov
{

namespace
{

struct IsAlnumChar
{
  bool operator()(unsigned char c) const
  {
    return std::isalnum(c) != 0;
  }
};

struct IsAlphaChar
{
  bool operator()(unsigned char c) const
  {
    return std::isalpha(c) != 0;
  }
};

struct IsNotAlphaChar
{
  bool operator()(char c) const
  {
    return IsAlphaChar{}(static_cast<unsigned char>(c)) == 0;
  }
};

struct ToLowerChar
{
  char operator()(char c) const
  {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
};

struct IsDigitChar
{
  bool operator()(unsigned char c) const
  {
    return std::isdigit(c) != 0;
  }
};

struct IsValidNameChar
{
  bool operator()(unsigned char c) const
  {
    return std::isalnum(c) || c == '_' || c == '-';
  }
};

struct AccumMaxPos
{
  size_t operator()(size_t acc, const std::pair<const std::string, std::vector<size_t>> &p) const
  {
    if (p.second.empty()) {
      return acc;
    }
    return std::max(acc, static_cast<size_t>(*std::max_element(p.second.begin(), p.second.end())));
  }
};

struct JoinWithSpace
{
  std::string operator()(const std::string &acc, const std::string &s) const
  {
    return acc + " " + s;
  }
};

struct PushToRefs
{
  std::map<std::string, std::vector<size_t>> *refs;
  size_t *idx;

  void operator()(std::string word) const
  {
    word.erase(std::remove_if(word.begin(), word.end(), IsNotAlphaChar{}), word.end());
    if (!word.empty()) {
      std::transform(word.begin(), word.end(), word.begin(), ToLowerChar{});
      (*refs)[word].push_back(*idx);
    }
    ++(*idx);
  }
};

struct PrintWithComma
{
  std::ostream *out;
  bool first;

  void operator()(size_t v)
  {
    if (!first) {
      (*out) << ",";
    }
    (*out) << v;
    first = false;
  }
};

struct PrintRefLine
{
  std::ofstream *out;

  void operator()(const std::pair<const std::string, std::vector<size_t>> &p)
  {
    (*out) << p.first << ":";
    PrintWithComma printer{ out, true };
    std::for_each(p.second.begin(), p.second.end(), printer);
    (*out) << "\n";
  }
};

struct ToCountPair
{
  std::pair<std::string, size_t> operator()(const std::pair<const std::string, std::vector<size_t>> &p) const
  {
    return std::make_pair(p.first, p.second.size());
  }
};

struct ByCountDesc
{
  bool operator()(const std::pair<std::string, size_t> &a,
                  const std::pair<std::string, size_t> &b) const
  {
    return a.second > b.second;
  }
};

struct PrintCountLine
{
  void operator()(const std::pair<std::string, size_t> &p) const
  {
    std::cout << "  " << p.first << ": " << p.second << std::endl;
  }
};

struct PrintPosSpace
{
  std::ostream *out;
  void operator()(size_t v) const
  {
    (*out) << v << " ";
  }
};

struct MergeAccum
{
  CrossReferenceSystem::TextData *dst;
  size_t offset;

  void operator()(const std::pair<const std::string, std::vector<size_t>> &pr) const
  {
    const std::string &w = pr.first;
    const std::vector<size_t> &pos = pr.second;
    std::vector<size_t> shifted;
    shifted.reserve(pos.size());
    std::transform(pos.begin(), pos.end(), std::back_inserter(shifted),
                   Offset{ offset });
    auto it = dst->references.find(w);
    if (it != dst->references.end()) {
      it->second.insert(it->second.end(), shifted.begin(), shifted.end());
    } else {
      dst->references[w] = shifted;
    }
  }

  struct Offset
  {
    size_t off;
    size_t operator()(size_t p) const { return p + off; }
  };
};

}

bool CrossReferenceSystem::isValidName(const std::string &name)
{
  if (name.empty()) {
    return false;
  }
  return std::all_of(name.begin(), name.end(),
                     [](char c){ return IsValidNameChar{}(static_cast<unsigned char>(c)); });
}

bool CrossReferenceSystem::isValidWord(const std::string &word)
{
  if (word.empty()) {
    return false;
  }
  return std::all_of(word.begin(), word.end(),
                     [](char c){ return IsAlphaChar{}(static_cast<unsigned char>(c)); });
}

std::vector<std::string> CrossReferenceSystem::splitIntoWords(const std::string &content)
{
  std::istringstream iss(content);
  return std::vector<std::string>(std::istream_iterator<std::string>(iss),
                                  std::istream_iterator<std::string>());
}

std::string CrossReferenceSystem::joinWords(const std::vector<std::string> &words)
{
  if (words.empty()) {
    return std::string();
  }
  return std::accumulate(std::next(words.begin()), words.end(), words.front(), JoinWithSpace{});
}

void CrossReferenceSystem::buildReferences(const std::string &text_name, const std::string &content)
{
  TextData &data = texts[text_name];
  data.content = content;
  data.references.clear();
  std::vector<std::string> words = splitIntoWords(content);
  size_t idx = 0;
  PushToRefs push{ &data.references, &idx };
  std::for_each(words.begin(), words.end(), push);
}

void CrossReferenceSystem::build(const std::string &text_name, const std::string &text_content)
{
  if (!isValidName(text_name)) {
    throw std::runtime_error("<INVALID NAME>");
  }
  if (text_content.empty()) {
    throw std::runtime_error("<EMPTY TEXT>");
  }
  if (texts.find(text_name) != texts.end()) {
    throw std::runtime_error("<ALREADY EXISTS>");
  }
  buildReferences(text_name, text_content);
}

void CrossReferenceSystem::reconstruct(const std::string &text_name, const std::string &output_file)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  const TextData &data = it->second;
  std::vector<std::string> words = splitIntoWords(data.content);

  size_t max_position = std::accumulate(data.references.begin(), data.references.end(), size_t{0}, AccumMaxPos{});
  if (max_position >= words.size()) {
    throw std::runtime_error("<CORRUPTED INDEX>");
  }

  if (output_file.empty()) {
    std::cout << data.content << std::endl;
  } else {
    std::ofstream outfile(output_file);
    if (!outfile) {
      throw std::runtime_error("<IO ERROR>");
    }
    if (std::ifstream(output_file)) {
      throw std::runtime_error("<IO EXISTS>");
    }
    PrintRefLine pr{ &outfile };
    std::for_each(data.references.begin(), data.references.end(), pr);
    outfile.close();
  }
}

void CrossReferenceSystem::concat(const std::string &new_name, const std::string &name1, const std::string &name2)
{
  if (!isValidName(new_name)) {
    throw std::runtime_error("<INVALID NAME>");
  }
  if (texts.find(new_name) != texts.end()) {
    throw std::runtime_error("<ALREADY EXISTS>");
  }

  auto it1 = texts.find(name1);
  auto it2 = texts.find(name2);
  if (it1 == texts.end()) {
    throw std::runtime_error(std::string("<NOT FOUND: ") + name1 + ">");
  }
  if (it2 == texts.end()) {
    throw std::runtime_error(std::string("<NOT FOUND: ") + name2 + ">");
  }

  std::string combined = it1->second.content + " " + it2->second.content;
  buildReferences(new_name, combined);
}

std::vector<size_t> CrossReferenceSystem::search(const std::string &text_name, const std::string &word)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }
  if (word.empty()) {
    throw std::runtime_error("<EMPTY WORD>");
  }

  std::string key = word;
  std::transform(key.begin(), key.end(), key.begin(), ToLowerChar{});
  const auto &refs = it->second.references;
  auto ref_it = refs.find(key);
  if (ref_it != refs.end()) {
    return ref_it->second;
  }
  return {};
}

void CrossReferenceSystem::replace(const std::string &text_name,
                                   const std::string &old_word,
                                   const std::string &new_word)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  std::string search_old = old_word;
  std::transform(search_old.begin(), search_old.end(), search_old.begin(), ToLowerChar{});

  TextData &data = it->second;
  auto ref_it = data.references.find(search_old);
  if (ref_it == data.references.end()) {
    throw std::runtime_error("<WORD NOT FOUND>");
  }

  std::vector<std::string> words = splitIntoWords(data.content);

  struct SetWordAt
  {
    std::vector<std::string> *words;
    const std::string *value;

    void operator()(size_t pos) const
    {
      if (pos < words->size()) {
        (*words)[pos] = *value;
      }
    }
  };

  std::for_each(ref_it->second.begin(), ref_it->second.end(), SetWordAt{ &words, &new_word });

  std::string new_content = joinWords(words);
  buildReferences(text_name, new_content);
}

void CrossReferenceSystem::insert(const std::string &text_name, size_t position, const std::string &word)
{
  if (!isValidWord(word)) {
    throw std::runtime_error("<INVALID WORD>");
  }

  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  TextData &data = it->second;
  std::vector<std::string> words = splitIntoWords(data.content);

  if (position > words.size()) {
    throw std::runtime_error("<INVALID POSITION>");
  }

  words.insert(words.begin() + position, word);
  std::string new_content = joinWords(words);
  buildReferences(text_name, new_content);
}

void CrossReferenceSystem::remove(const std::string &text_name, size_t start, size_t end)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  TextData &data = it->second;
  std::vector<std::string> words = splitIntoWords(data.content);

  if (start >= words.size() || end >= words.size() || start > end) {
    throw std::runtime_error("<INVALID RANGE>");
  }

  words.erase(words.begin() + start, words.begin() + end + 1);
  std::string new_content = joinWords(words);
  buildReferences(text_name, new_content);
}

void CrossReferenceSystem::import(const std::string &filename)
{
  std::ifstream infile(filename);
  if (!infile) {
    throw std::runtime_error("<FILE NOT FOUND>");
  }

  std::ostringstream oss;
  oss << infile.rdbuf();
  std::string content = oss.str();

  if (content.empty()) {
    throw std::runtime_error("<INVALID FORMAT>");
  }

  std::string text_name = filename;
  size_t dot_pos = text_name.find_last_of('.');
  if (dot_pos != std::string::npos) {
    text_name = text_name.substr(0, dot_pos);
  }

  build(text_name, content);
}

void CrossReferenceSystem::export_text(const std::string &text_name, const std::string &filename)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  std::ofstream outfile(filename);
  if (!outfile) {
    throw std::runtime_error("<IO ERROR>");
  }

  PrintRefLine pr{ &outfile };
  std::for_each(it->second.references.begin(), it->second.references.end(), pr);
  outfile.close();
}

void CrossReferenceSystem::stats(const std::string &text_name)
{
  auto it = texts.find(text_name);
  if (it == texts.end()) {
    throw std::runtime_error("<NOT FOUND>");
  }

  const TextData &data = it->second;
  std::vector<std::string> words = splitIntoWords(data.content);

  std::cout << "Total words: " << data.references.size() << std::endl;
  std::cout << "Total occurrences: " << words.size() << std::endl;

  std::vector<std::pair<std::string, size_t>> word_counts;
  word_counts.reserve(data.references.size());
  std::transform(data.references.begin(), data.references.end(),
                 std::back_inserter(word_counts), ToCountPair{});

  std::sort(word_counts.begin(), word_counts.end(), ByCountDesc{});
  if (word_counts.size() > 5) {
    word_counts.resize(5);
  }
  std::for_each(word_counts.begin(), word_counts.end(), PrintCountLine{});
}

void CrossReferenceSystem::merge(const std::string &new_name,
                                 const std::string &name1,
                                 const std::string &name2)
{
  if (!isValidName(new_name)) {
    throw std::runtime_error("<INVALID NAME>");
  }
  if (texts.find(new_name) != texts.end()) {
    throw std::runtime_error("<ALREADY EXISTS>");
  }

  auto it1 = texts.find(name1);
  if (it1 == texts.end()) {
    throw std::runtime_error(std::string("<NOT FOUND: ") + name1 + ">");
  }
  auto it2 = texts.find(name2);
  if (it2 == texts.end()) {
    throw std::runtime_error(std::string("<NOT FOUND: ") + name2 + ">");
  }

  const TextData &data1 = it1->second;
  const TextData &data2 = it2->second;

  std::vector<std::string> words1 = splitIntoWords(data1.content);
  size_t word_count1 = words1.size();

  TextData new_data;
  new_data.content = data1.content + " " + data2.content;
  new_data.references = data1.references;

  MergeAccum acc{ &new_data, word_count1 };
  std::for_each(data2.references.begin(), data2.references.end(), acc);

  texts[new_name] = new_data;
}

int processCommandLineArguments(int argc, char **argv, CrossReferenceSystem &system)
{
  if (argc > 1) {
    std::string arg = argv[1];
    if (arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [file] | --help | --check <file> | --export <file>\n";
      return 0;
    } else if (arg == "--check" && argc > 2) {
      try {
        system.import(argv[2]);
        std::cout << "File is valid" << std::endl;
        return 0;
      } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
      }
    } else if (arg == "--export" && argc > 2) {
      return 0;
    } else {
      try {
        system.import(argv[1]);
        return 0;
      } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
      }
    }
  }
  return -1;
}

void runInteractiveMode(CrossReferenceSystem &system)
{
  std::string command;
  for (;;) {
    std::cout << "> ";
    if (!std::getline(std::cin, command)) {
      break;
    }

    if (command.empty()) {
      continue;
    }

    size_t first_space = command.find(' ');
    std::string cmd = (first_space == std::string::npos) ? command : command.substr(0, first_space);
    std::string args = (first_space == std::string::npos) ? "" : command.substr(first_space + 1);

    try {
      if (cmd == "build") {
        size_t name_end = args.find(' ');
        if (name_end == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, name_end);
        std::string content = args.substr(name_end + 1);
        system.build(name, content);
        std::cout << "OK" << std::endl;
      } else if (cmd == "reconstruct") {
        size_t name_end = args.find(' ');
        if (name_end == std::string::npos) {
          system.reconstruct(args);
        } else {
          std::string name = args.substr(0, name_end);
          std::string output_file = args.substr(name_end + 1);
          system.reconstruct(name, output_file);
        }
        std::cout << "OK" << std::endl;
      } else if (cmd == "concat") {
        size_t pos1 = args.find(' ');
        size_t pos2 = args.find(' ', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string new_name = args.substr(0, pos1);
        std::string name1 = args.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string name2 = args.substr(pos2 + 1);
        system.concat(new_name, name1, name2);
        std::cout << "OK" << std::endl;
      } else if (cmd == "search") {
        size_t pos = args.find(' ');
        if (pos == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, pos);
        std::string word = args.substr(pos + 1);
        auto positions = system.search(name, word);
        if (positions.empty()) {
          std::cout << "Word not found" << std::endl;
        } else {
          PrintPosSpace printer{ &std::cout };
          std::cout << "Positions: ";
          std::for_each(positions.begin(), positions.end(), printer);
          std::cout << std::endl;
        }
      } else if (cmd == "replace") {
        size_t pos1 = args.find(' ');
        size_t pos2 = args.find(' ', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, pos1);
        std::string old_word = args.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string new_word = args.substr(pos2 + 1);
        system.replace(name, old_word, new_word);
        std::cout << "OK" << std::endl;
      } else if (cmd == "insert") {
        size_t pos1 = args.find(' ');
        size_t pos2 = args.find(' ', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, pos1);
        size_t position = std::stoul(args.substr(pos1 + 1, pos2 - pos1 - 1));
        std::string word = args.substr(pos2 + 1);
        system.insert(name, position, word);
        std::cout << "OK" << std::endl;
      } else if (cmd == "remove") {
        size_t pos1 = args.find(' ');
        size_t pos2 = args.find(' ', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, pos1);
        size_t start = std::stoul(args.substr(pos1 + 1, pos2 - pos1 - 1));
        size_t end = std::stoul(args.substr(pos2 + 1));
        system.remove(name, start, end);
        std::cout << "OK" << std::endl;
      } else if (cmd == "import") {
        system.import(args);
        std::cout << "OK" << std::endl;
      } else if (cmd == "export") {
        size_t pos = args.find(' ');
        if (pos == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string name = args.substr(0, pos);
        std::string filename = args.substr(pos + 1);
        system.export_text(name, filename);
        std::cout << "OK" << std::endl;
      } else if (cmd == "stats") {
        system.stats(args);
      } else if (cmd == "merge") {
        size_t pos1 = args.find(' ');
        size_t pos2 = args.find(' ', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
          throw std::runtime_error("<INVALID ARGUMENTS>");
        }
        std::string new_name = args.substr(0, pos1);
        std::string name1 = args.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string name2 = args.substr(pos2 + 1);
        system.merge(new_name, name1, name2);
        std::cout << "OK" << std::endl;
      } else if (cmd == "exit" || cmd == "quit") {
        break;
      } else {
        std::cout << "Unknown command: " << cmd << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
}

}
