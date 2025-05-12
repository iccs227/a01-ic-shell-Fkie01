/* ICCS227: Project 1: icsh
 * Name: Subhakorn Chayawatto 
 * StudentID: 6680597
 */

#include "parser.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    std::string input;
    std::string lastCommand;
    int exitCode = 0;

    std::cout << "  -------------welcome to icsh shell by me <;<--------------\n";
    

    while(true){
        std::cout << "icsh $ ";
        std::getline(std::cin, input);

        // Handle !!
        if (input == "!!") {
            if (lastCommand.empty())
                continue;
            std::cout << lastCommand << std::endl;
            input = lastCommand;
        } else {
            lastCommand = input;
        }

        

        std::vector<std::string> args = parseInput(input);
        if (args.empty()) {
            continue;
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

        std::vector<char*> cargs;
        for (auto &arg : args) {
            cargs.push_back(const_cast<char*>(arg.c_str()));
        }
        cargs.push_back(nullptr);

        pid_t pid = fork();
        if (pid == 0) {
      // child process
            if (execvp(cargs[0], cargs.data()) == -1) {
                // perror("Bad Command");
                std::cout << "Bad command: noob! \n";
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // parent process
            waitpid(pid, nullptr, 0);
        } else {
            perror("fork failed");
        }
        
    }
    return exitCode;
}