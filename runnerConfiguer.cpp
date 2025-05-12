

#include "parser.hpp"
#include "execute.hpp"
#include "signalHandle.hpp"
#include "isRedirection.hpp"
#include "jobs.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64


// HANDLE DOUBLE BANG
std::string checkDoubleBang(std::string &input, std::string &lastCommand) {
    if (input == "!!") {
        input = lastCommand;
        std::cout << input << std::endl;
    } else {
        lastCommand = input;
    }
    return input;
}


bool checkIfRedirection(std::vector<std::string> &args) {
    // Check for redirection
        if (args.size() >= 1) {
            for (size_t i = 1; i < args.size(); ++i) {
                if (std::string(args[i]) == ">" || std::string(args[i]) == "<") {
                return true;
                break;
                }
            }
        }
        return false;
}

void echoCommand(std::vector<std::string> &args) {
    if (args.size() >= 1 && strcmp(args[0].c_str(), "echo") == 0) {
        for (size_t i = 1; i < args.size(); ++i) {
            std::cout << args[i] << " ";
        }
        std::cout << std::endl;
    }
}

