#include "controller.h"
#include <algorithm>
#include <fstream>


const std::string Digits = "012345678"; //строка для проверки правил

int checkNum(const std::string &num) { //проверяем число и преобразовываем в инт
    if (num.empty() || (num[0] == '0' && num.size() > 1)) {
        throw std::invalid_argument("Ticks are zero or negatice");
    }
    for (size_t i = 0; i < num.size(); ++i) {
        if (!isdigit(num[i])) {
            throw std::invalid_argument("Tick incorrect");
        }
    }
    //str->int
    return stoi(num);
}

std::string checkRule(const std::string &rule) { //проверяем правила
    if (rule.empty()) {
        throw std::invalid_argument("Rule is incorrect");
    }
    for (const auto &i: rule) {
        if (Digits.find(i) == Digits.npos) {
            throw std::invalid_argument("Rule is incorrect");
        }
    }
    return rule;
}

void printInFile(
        std::ofstream &out,
        const Field &field,
        const std::string &name,
        int glob_iteration
) {
    for (int i = 0; i <= field.getSize(); ++i) {
        for (int j = 0; j <= field.getSize(); ++j) {
            if (field.getState(i, j)) {
                out << "* ";
            } else {
                out << ". ";
            }
        }
        out << std::endl;
    }
}

SimulationController::SimulationController() {
    model_ = std::make_unique<Game_model>();
}

SimulationController::SimulationController(
        const std::string& inf
): SimulationController(inf, "1", "") {} //1 тик по дефолту

SimulationController::SimulationController(
        const std::string& infile,
        const std::string& ticks,
        const std::string& outfile
) {
    int tick;
    std::string name;
    std::string b_rule;
    std::string s_rule;
    std::string str;
    std::vector<Point> coords;
    std::ifstream ifile (infile);
    try {
        if (!ifile.is_open()) {
            throw std::invalid_argument("You inserted the wrong in file");
        }
        
        getline(ifile, str); //название
        if (str.substr(0, 3) != "#N ") {
            throw std::invalid_argument("You inserted the wrong name of universe");
        } else {
            name = str.substr(3, str.size() - 3);
        }
        getline(ifile, str); //правила
        if (str.substr(0, 4) != "#R B") {
            throw std::invalid_argument("There are no transition rules");
        } else {
            size_t pos = str.find('/');
            if (pos == str.npos || str[pos + 1] != 'S') {
                throw std::invalid_argument("You entered the wrong rules");
            }
            b_rule = checkRule(str.substr(4, pos - 4));
            s_rule = checkRule(str.substr(pos + 2, str.size() - pos - 2));
        }
        while (getline(ifile, str)) { //координаты
            size_t pos = str.find(' ');
            if (pos == 0 || pos == str.npos) {
                throw std::invalid_argument("Not enough coordinates");
            }
            int x = checkNum(str.substr(0, pos));
            int y = checkNum(str.substr(pos + 1, str.size() - pos - 1));
            coords.push_back({x, y});
        }
        out_file = outfile;
        model_ = std::make_unique<Game_model>(name, b_rule, s_rule, coords);
    } catch (std::invalid_argument& err) {
        view_.printErr(err.what());
    }
}

void SimulationController::play_game() {
    try {
        if (!out_file.empty()) {
            std::ofstream ofile (out_file);
            if (!ofile.is_open())
            {
                throw std::invalid_argument("Fike isn't open");
            }
            for (int i = 0; i < model_->getGlobIteration(); ++i) {
                model_->computeIteration();
            }
            printInFile(
                    ofile, 
                    model_->getField(),
                    model_->getName(), 
                    model_->getGlobIteration()
            );
            return;
        }
        std::string command = "help";
        while (command != "exit") {
            if (command.substr(0, 4) == "tick") {
                int num_tick;
                if (command.size() < 4) {
                    num_tick = 1;
                } else {
                    try {
                        num_tick = checkNum(command.substr(5, command.size() - 5));
                    } catch (std::invalid_argument) {
                        num_tick = 1;
                    }
                }
                for (int i = 1; i <= num_tick; ++i) {
                    view_.printFrame(
                            model_->getName(), 
                            model_->getField(),
                            model_->getGlobIteration() + 1, i
                    );
                    model_->computeIteration();
                }
            } else if (command.substr(0, 4) == "dump") {
                if (command.substr(command.size() - 5, 5) == ".txt") {
                    view_.printErr("Wrong out file");
                } else {
                    std::ofstream ofile(command.substr(5, command.size()- 5));
                    if (ofile.is_open()) {
                        printInFile(
                                ofile, model_->getField(), model_->getName(),
                                model_->getGlobIteration()
                        );
                    }
                }
            } else {
                view_.printHelp();
            }
            getline(std::cin, command);
        }
    } catch (std::invalid_argument& err) {
        view_.printErr(err.what());
       // std::rethrow_exception(std::current_exception());
    }
};