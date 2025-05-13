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
#include <ctime>

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
    std::string currentTime = getCurrentTime();
    if(configEnabled("show_git_branch")) {
        std::string branch = getGitBranch();
        if (configEnabled("show_prompt_path")) {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            std::cout << " "; 
            std::cout << "\033[1;31;43m"; // Bright cyan text on red background
            std::cout << " "  <<  currentTime << " " ;
            std::cout << "\033[0m"; // Reset to normal

            std::cout << "\033[1;30;42m"; // Bright cyan text on red background
            std::cout << " " <<  branch << " " ;
            std::cout << "\033[0m"; // Reset to normal



            std::cout << "\033[1;30;47m"; // Bright cyan text on red background
            std::cout <<  " " << cwd << "  icsh" << " $ ";
            std::cout << "\033[0m"; // Reset to normal
            std::cout << std::endl;
        } else {
            std::cout << " "; 
            std::cout << "\033[1;31;43m"; // Bright cyan text on red background
            std::cout << " "  <<  currentTime << " " ;
            std::cout << "\033[0m"; // Reset to normal


            std::cout << "\033[1;30;42m"; // Bright cyan text on red background
            std::cout << " " <<  branch << " " ;
            std::cout << "\033[0m"; // Reset to normal



            std::cout << "\033[1;30;47m"; // Bright cyan text on red background
            std::cout <<"  icsh $ ";
            std::cout << "\033[0m"; // Reset to normal
            std::cout << std::endl;
        }
    } else {
        if (configEnabled("show_prompt_path")) {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            std::cout << " "; 
            std::cout << "\033[1;31;43m"; // Bright cyan text on red background
            std::cout << " "  <<  currentTime << " " ;
            std::cout << "\033[0m"; // Reset to normal

            // std::cout << "\033[1;30;42m"; // Bright cyan text on red background
            // std::cout << " " <<  branch << " " ;
            // std::cout << "\033[0m"; // Reset to normal



            std::cout << "\033[1;30;47m"; // Bright cyan text on red background
            std::cout <<  " " << cwd << "  icsh" << " $ ";
            std::cout << "\033[0m"; // Reset to normal
            std::cout << std::endl;
        } else {
            std::cout << " "; 
            std::cout << "\033[1;31;43m"; // Bright cyan text on red background
            std::cout << " "  <<  currentTime << " " ;
            std::cout << "\033[0m"; // Reset to normal


            // std::cout << "\033[1;30;42m"; // Bright cyan text on red background
            // std::cout << " " <<  branch << " " ;
            // std::cout << "\033[0m"; // Reset to normal



            std::cout << "\033[1;30;47m"; // Bright cyan text on red background
            std::cout <<"  icsh $ ";
            std::cout << "\033[0m"; // Reset to normal
            std::cout << std::endl;
        }
    }
}

std::string getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%H:%M:%S");  // Format: HH:MM:SS
    return oss.str();
}

std::string getGitBranch() {
    std::string branch;
    FILE* fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");

    if (fp) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), fp)) {
            branch = buffer;
            // Remove newline
            branch.erase(branch.find_last_not_of("\n\r") + 1);
        }
        pclose(fp);
    }
    return branch;
}