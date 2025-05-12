/* ICCS227: Project 1: icsh
 * Name: Subhakorn Chayawatto 
 * StudentID: 6680597
 */

#include "parser.hpp"
#include "execute.hpp"
#include "signalHandle.hpp"
#include "isRedirection.hpp"
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

int main(int argc, char *argv[]) {
    setupSignalHandlers(); // handle ctrl-c and ctrl-z
    std::istream *inputStream = &std::cin;
    std::ifstream fileStream;
    

    if (argc == 2){
        fileStream.open(argv[1]);
        if(!fileStream.is_open()){
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        inputStream = &fileStream;
    }

    std::string input;
    std::string output;
    std::string lastCommand;
    int exitCode = 0;


    std::cout << "  -------------welcome to icsh shell by me ;_; <--------------\n";
    

    while(true){
        bool isRedirection = false;

        if(inputStream == &std::cin){
            std::cout << "icsh $ ";
        }

        if(!std::getline(*inputStream, input)){
            if(!inputStream) break;
        }

        // Handle !!
        if (input == "!!") {
            if (lastCommand.empty()){
                std::cerr << "No previous command to repeat." << std::endl;
                continue;
            }
            std::cout << lastCommand << std::endl;
            input = lastCommand;
        } else {
            lastCommand = input;
        }

        

        std::vector<std::string> args = parseInput(input);
        if (args.empty()) {
            continue;
        }
        // Check for redirection
        if (args.size() >= 1) {
            for (size_t i = 1; i < args.size(); ++i) {
                if (std::string(args[i]) == ">" || std::string(args[i]) == "<") {
                isRedirection = true;
                break;
                }
            }
        }

        // Handle exit with optional code
        if (args[0] == "exit") {
            if (args.size() == 2) {
                try {
                exitCode = std::stoi(args[1]) & 0xFF;
                } catch (...) {
                exitCode = 0;
                }
            }
            std::cout << "bye\n";
            break;
        }

        char command_line[MAX_LINE];
        char *argss[MAX_ARGS];
        RedirectionType redir_type;
        char *fileName;
        std::strncpy(command_line, input.c_str(), MAX_LINE);
        
        if(isRedirection){
            parse_command(command_line, argss, &redir_type, &fileName);
            handleRedirectionAndExecute(argss, redir_type, fileName);
        } else {
            exitCode = executeCommand(args);
        }

        
        
    }
    return exitCode;
}