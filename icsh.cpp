/* ICCS227: Project 1: icsh
 * Name: Subhakorn Chayawatto 
 * StudentID: 6680597
 */

#include "parser.hpp"
#include "execute.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
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
    std::string lastCommand;
    int exitCode = 0;


    std::cout << "  -------------welcome to icsh shell by me <;<--------------\n";
    

    while(true){

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

        exitCode = executeCommand(args);
        
    }
    return exitCode;
}