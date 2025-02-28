#include <iostream>
#include <vector>
#include <string>
#include "controller/controller.h"
#include "model/model.h"
#include "view/view.h"
#include <gtest/gtest.h>

std::vector<std::string> EditArgs(int argc, char** argv) {
    std::vector<std::string> args(argc);
    for (int i = 0; i < argc; ++i) {
        args[i] = argv[i];
    }
    return args;
}

int main(int argc, char** argv) {
    std::unique_ptr<SimulationController> life;
    std::vector<std::string> args = EditArgs(argc, argv);
    try {
        if (argc == 1) {
            life = std::make_unique<SimulationController>();
        } else if (argc == 2) {
            life = std::make_unique<SimulationController>(args[1]);
        } else if (argc == 6) {
            int i_num = 1, o_num = 1;
            for (int i = 0; i < argc; ++i) {
                if (args[i] == "-o") {
                    o_num += i;
                } else if (args[i] == "-i") {
                    i_num += i;
                }
            }
            if (i_num > 2 && o_num > 2) {
                life = std::make_unique<SimulationController>(args[1], args[i_num], args[o_num]);
            } else {
                std::cout << "Wrong keys";
            }
        } else {
            std::cout << "Wrong keys";
            return 1;
        }
    } catch (std::invalid_argument) {
        return 1;
    }

    life->play_game();
    return 0;
}