#include "config.hpp"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <sys/wait.h>
#include <fstream>
#include <map>

std::map<std::string, std::string> config;

void loadConfig() {

    std::ifstream file(".icshrc");

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open ~/.myshrc config\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string key, val;
        if (std::getline(iss, key, '=') && std::getline(iss, val)) {
            config[key] = val;
        }
    }
}

bool configEnabled(const std::string& key) {
    return config.count(key) && config[key] == "1";
}

void showPrompt() {
    if (configEnabled("show_prompt_path")) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        std::cout << "\033[1;30;47m"; // Bright cyan text on red background
        std::cout <<  "" << cwd << "  icsh" << " $ ";
        std::cout << "\033[0m"; // Reset to normal
    } else {
        std::cout << "\033[1;30;47m"; // Bright cyan text on red background
        std::cout << "  icsh $ ";
        std::cout << "\033[0m"; // Reset to normal
    }
}