#include "Xref.h"

namespace sherkunov {

bool CrossReferenceSystem::isValidName(const std::string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    return true;
}

bool CrossReferenceSystem::isValidWord(const std::string& word) {
    if (word.empty()) return false;
    for (char c : word) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

void CrossReferenceSystem::buildReferences(const std::string& text_name, const std::string& content) {
    TextData& data = texts[text_name];
    data.content = content;
    data.references.clear();

    std::istringstream iss(content);
    std::string word;
    size_t position = 0;

    while (iss >> word) {
        // Очищаем слово от знаков препинания
        word.erase(std::remove_if(word.begin(), word.end(),
                 [](char c) { return !std::isalpha(c); }), word.end());
        if (!word.empty()) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            data.references[word].push_back(position);
        }
        position++;
    }
}

void CrossReferenceSystem::build(const std::string& text_name, const std::string& text_content) {
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

void CrossReferenceSystem::reconstruct(const std::string& text_name, const std::string& output_file) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    const TextData& data = it->second;
    std::istringstream iss(data.content);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    for (const auto& ref : data.references) {
        for (size_t pos : ref.second) {
            if (pos >= words.size()) {
                throw std::runtime_error("<CORRUPTED INDEX>");
            }
        }
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
        outfile << data.content;
        outfile.close();
    }
}

void CrossReferenceSystem::concat(const std::string& new_name, const std::string& name1, const std::string& name2) {
    if (!isValidName(new_name)) {
        throw std::runtime_error("<INVALID NAME>");
    }
    if (texts.find(new_name) != texts.end()) {
        throw std::runtime_error("<ALREADY EXISTS>");
    }

    auto it1 = texts.find(name1);
    if (it1 == texts.end()) {
        throw std::runtime_error("<NOT FOUND: " + name1 + ">");
    }
    auto it2 = texts.find(name2);
    if (it2 == texts.end()) {
        throw std::runtime_error("<NOT FOUND: " + name2 + ">");
    }

    std::string combined_content = it1->second.content + " " + it2->second.content;
    buildReferences(new_name, combined_content);
}

std::vector<size_t> CrossReferenceSystem::search(const std::string& text_name, const std::string& word) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }
    if (word.empty()) {
        throw std::runtime_error("<EMPTY WORD>");
    }

    std::string search_word = word;
    std::transform(search_word.begin(), search_word.end(), search_word.begin(), ::tolower);

    const auto& refs = it->second.references;
    auto ref_it = refs.find(search_word);
    if (ref_it != refs.end()) {
        return ref_it->second;
    }
    return {};
}

void CrossReferenceSystem::replace(const std::string& text_name, const std::string& old_word, const std::string& new_word) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    std::string search_old_word = old_word;
    std::transform(search_old_word.begin(), search_old_word.end(), search_old_word.begin(), ::tolower);

    TextData& data = it->second;
    auto ref_it = data.references.find(search_old_word);
    if (ref_it == data.references.end()) {
        throw std::runtime_error("<WORD NOT FOUND>");
    }

    std::istringstream iss(data.content);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    for (size_t pos : ref_it->second) {
        if (pos < words.size()) {
            words[pos] = new_word;
        }
    }

    std::ostringstream oss;
    for (size_t i = 0; i < words.size(); ++i) {
        if (i > 0) oss << " ";
        oss << words[i];
    }
    buildReferences(text_name, oss.str());
}

void CrossReferenceSystem::insert(const std::string& text_name, size_t position, const std::string& word) {
    if (!isValidWord(word)) {
        throw std::runtime_error("<INVALID WORD>");
    }

    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    TextData& data = it->second;
    std::istringstream iss(data.content);
    std::vector<std::string> words;
    std::string current_word;
    while (iss >> current_word) {
        words.push_back(current_word);
    }

    if (position > words.size()) {
        throw std::runtime_error("<INVALID POSITION>");
    }

    words.insert(words.begin() + position, word);

    std::ostringstream oss;
    for (size_t i = 0; i < words.size(); ++i) {
        if (i > 0) oss << " ";
        oss << words[i];
    }

    buildReferences(text_name, oss.str());
}

void CrossReferenceSystem::remove(const std::string& text_name, size_t start, size_t end) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    TextData& data = it->second;
    std::istringstream iss(data.content);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

    if (start >= words.size() || end >= words.size() || start > end) {
        throw std::runtime_error("<INVALID RANGE>");
    }

    words.erase(words.begin() + start, words.begin() + end + 1);

    std::ostringstream oss;
    for (size_t i = 0; i < words.size(); ++i) {
        if (i > 0) oss << " ";
        oss << words[i];
    }

    buildReferences(text_name, oss.str());
}

void CrossReferenceSystem::import(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("<FILE NOT FOUND>");
    }

    std::string content((std::istreambuf_iterator<char>(infile)),
                       std::istreambuf_iterator<char>());

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

void CrossReferenceSystem::export_text(const std::string& text_name, const std::string& filename) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    std::ofstream outfile(filename);
    if (!outfile) {
        throw std::runtime_error("<IO ERROR>");
    }

    for (const auto& ref : it->second.references) {
        outfile << ref.first << ":";
        for (size_t i = 0; i < ref.second.size(); ++i) {
            if (i > 0) outfile << ",";
            outfile << ref.second[i];
        }
        outfile << "\n";
    }

    outfile.close();
}

void CrossReferenceSystem::stats(const std::string& text_name) {
    auto it = texts.find(text_name);
    if (it == texts.end()) {
        throw std::runtime_error("<NOT FOUND>");
    }

    const TextData& data = it->second;
    std::cout << "Total words: " << data.references.size() << std::endl;
    std::cout << "Total occurrences: " << data.content.size() << std::endl;

    std::vector<std::pair<std::string, size_t>> word_counts;
    for (const auto& ref : data.references) {
        word_counts.emplace_back(ref.first, ref.second.size());
    }

    std::sort(word_counts.begin(), word_counts.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "Top 5 words:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), word_counts.size()); ++i) {
        std::cout << "  " << word_counts[i].first << ": " << word_counts[i].second << std::endl;
    }
}

void CrossReferenceSystem::double_replace(const std::string& text1_name, const std::string& text2_name,
                   const std::string& word1, const std::string& word2) {
    if (text1_name == text2_name) {
        throw std::runtime_error("<SAME TEXT>");
    }

    auto it1 = texts.find(text1_name);
    if (it1 == texts.end()) {
        throw std::runtime_error("<NOT FOUND: " + text1_name + ">");
    }

    auto it2 = texts.find(text2_name);
    if (it2 == texts.end()) {
        throw std::runtime_error("<NOT FOUND: " + text2_name + ">");
    }

    std::string search_word1 = word1;
    std::transform(search_word1.begin(), search_word1.end(), search_word1.begin(), ::tolower);
    std::string search_word2 = word2;
    std::transform(search_word2.begin(), search_word2.end(), search_word2.begin(), ::tolower);

    TextData& data1 = it1->second;
    TextData& data2 = it2->second;

    auto ref_it1 = data1.references.find(search_word1);
    if (ref_it1 == data1.references.end()) {
        throw std::runtime_error("<WORD NOT FOUND IN TEXT1>");
    }

    auto ref_it2 = data2.references.find(search_word2);
    if (ref_it2 == data2.references.end()) {
        throw std::runtime_error("<WORD NOT FOUND IN TEXT2>");
    }

    std::istringstream iss1(data1.content);
    std::vector<std::string> words1;
    std::string word;
    while (iss1 >> word) {
        words1.push_back(word);
    }

    for (size_t pos : ref_it1->second) {
        if (pos < words1.size()) {
            words1[pos] = word2;
        }
    }

    std::istringstream iss2(data2.content);
    std::vector<std::string> words2;
    while (iss2 >> word) {
        words2.push_back(word);
    }

    for (size_t pos : ref_it2->second) {
        if (pos < words2.size()) {
            words2[pos] = word1;
        }
    }

    std::ostringstream oss1;
    for (size_t i = 0; i < words1.size(); ++i) {
        if (i > 0) oss1 << " ";
        oss1 << words1[i];
    }

    std::ostringstream oss2;
    for (size_t i = 0; i < words2.size(); ++i) {
        if (i > 0) oss2 << " ";
        oss2 << words2[i];
    }

    buildReferences(text1_name, oss1.str());
    buildReferences(text2_name, oss2.str());
}

int processCommandLineArguments(int argc, char** argv, CrossReferenceSystem& system) {
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
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        } else if (arg == "--export" && argc > 2) {
            return 0;
        } else {
            try {
                system.import(argv[1]);
                return 0;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        }
    }
    return -1;
}

void runInteractiveMode(CrossReferenceSystem& system) {
    std::string command;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, command)) {
            break;
        }

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        try {
            if (cmd == "build") {
                std::string name, content;
                iss >> name;
                std::getline(iss, content);
                if (!content.empty() && content[0] == ' ') {
                    content = content.substr(1);
                }
                system.build(name, content);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "reconstruct") {
                std::string name, output_file;
                iss >> name;
                if (iss >> output_file) {
                    system.reconstruct(name, output_file);
                } else {
                    system.reconstruct(name);
                }
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "concat") {
                std::string new_name, name1, name2;
                iss >> new_name >> name1 >> name2;
                system.concat(new_name, name1, name2);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "search") {
                std::string name, word;
                iss >> name >> word;
                auto positions = system.search(name, word);
                if (positions.empty()) {
                    std::cout << "Word not found" << std::endl;
                } else {
                    std::cout << "Positions: ";
                    for (size_t pos : positions) {
                        std::cout << pos << " ";
                    }
                    std::cout << std::endl;
                }
            }
            else if (cmd == "replace") {
                std::string name, old_word, new_word;
                iss >> name >> old_word >> new_word;
                system.replace(name, old_word, new_word);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "insert") {
                std::string name, word;
                size_t position;
                iss >> name >> position >> word;
                system.insert(name, position, word);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "remove") {
                std::string name;
                size_t start, end;
                iss >> name >> start >> end;
                system.remove(name, start, end);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "import") {
                std::string filename;
                iss >> filename;
                system.import(filename);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "export") {
                std::string name, filename;
                iss >> name >> filename;
                system.export_text(name, filename);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "stats") {
                std::string name;
                iss >> name;
                system.stats(name);
            }
            else if (cmd == "double_replace") {
                std::string text1, text2, word1, word2;
                iss >> text1 >> text2 >> word1 >> word2;
                system.double_replace(text1, text2, word1, word2);
                std::cout << "OK" << std::endl;
            }
            else if (cmd == "exit" || cmd == "quit") {
                break;
            }
            else {
                std::cout << "Unknown command: " << cmd << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

}
