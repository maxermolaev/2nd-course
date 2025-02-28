 #include <iostream> // библиотека ввода-вывода
#include <fstream> // библиотека для работы с файлами
#include <string> // для работы со строками
#include <list> // для хранения строк
#include <sstream> // для разбивания строки на слова
#include <map> // хранит пары ключ-значение для частоты
#include <algorithm> // для сортировки
#include <cctype> // для работы с символами

void processFile(const std::string& input, const std::string& output) {
    std::ifstream inFile(input);
    std::list<std::string> lines;
    std::map<std::string, int> wordCount;
    std::string line;

    while (std::getline(inFile, line)) {
        lines.push_back(line);
    }

    for (const auto& str : lines) {
        std::stringstream ss(str);
        std::string word;

        while (ss >> word) {
            word.erase(std::remove_if(word.begin(), word.end(), [](char c) {
                return !std::isalnum(c);
            }), word.end());
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (!word.empty()) {
                wordCount[word]++;
            }
        }
    }

    int totalWords = 0;
    for (const auto& pair : wordCount) {
        totalWords += pair.second;
    }

    std::vector<std::pair<std::string, int>> sortedWords(wordCount.begin(), wordCount.end());
    std::sort(sortedWords.begin(), sortedWords.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second;
        
    });

    std::ofstream outFile(output);
    outFile << "Слово, Частота, Частота (%)\n";
    for (const auto& pair : sortedWords) {
        double frequencePercent = (static_cast<double>(pair.second) / totalWords) * 100;
        outFile <<  pair.first << ", " <<  pair.second << ", " <<  frequencePercent << "%\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: word_count.exe input.txt output.csv\n";
        return 1;
    }

    const std::string input = argv[1];
    const std::string output = argv[2];

    processFile(input, output);
    return 0;
}
