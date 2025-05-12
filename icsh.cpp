/* ICCS227: Project 1: icsh
 * Name: Subhakorn Chayawatto 
 * StudentID: 6680597
 */

#include "parser.hpp"
#include "execute.hpp"
#include "signalHandle.hpp"
#include "isRedirection.hpp"
#include "jobs.hpp"
#include "runnerConfiguer.hpp"
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
    int exitCode = 0;
    

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
    


    std::cout << "  -------------welcome to icsh shell by me ;_; <--------------\n";
    

    while(true){
        bool isRedirection = false;
        bool background = false;

        if(inputStream == &std::cin){
            // std::cout << "icsh $ ";
            write(STDOUT_FILENO, "icsh $ ", 7);
            std::cout << std::flush;
        }

        if(!std::getline(*inputStream, input)){
            if(!inputStream) break;

            std::cin.clear();
            std::cout << std::endl;
            continue;
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
        input = checkDoubleBang(input, lastCommand);

        

        std::vector<std::string> args = parseInput(input);
        if (args.empty()) {
            continue;
        }
        // // Check for redirection
        // if (args.size() >= 1) {
        //     for (size_t i = 1; i < args.size(); ++i) {
        //         if (std::string(args[i]) == ">" || std::string(args[i]) == "<") {
        //         isRedirection = true;
        //         break;
        //         }
        //     }
        // }
        isRedirection = checkIfRedirection(args);

        

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


        // Handle echo $?
        if (args.size() >= 2 && strcmp(args[0].c_str(), "echo") == 0 && args[1] == "$?") {
            if (lastExitCode != 0){
                std::cout << lastExitCode << std::endl;
                lastExitCode = 0;
                continue;
            } else {
                std::cout << lastExitCode << std::endl;
                continue;
            }
            

        }
        // // Handle echo command
        // if (args.size() >= 1 && strcmp(args[0].c_str(), "echo") == 0) {
        //     for (size_t i = 1; i < args.size(); ++i) {
        //         std::cout << args[i] << " ";
        //     }
        //     std::cout << std::endl;
        //     continue;
        // }
        // echoCommand(args);

        // if (args.size() >= 1 && strcmp(args[0].c_str(), "jobs") == 0) { // print jobs 
        //     print_jobs();
        //     continue;
        // }



        if (args.size() >= 1 && strcmp(args[0].c_str(), "fg") == 0) { // bring to foreground
            if (args.size() == 2) {

                if (args[1][0] == '%') {
                args[1].erase(0, 1); // Remove the '%' character
                }

                // std::cout << args[1] << std::endl;
                int jobId = std::stoi(args[1]);
                
                bringToForeground(jobId);
            } else {
                std::cerr << "Usage: fg <job_id>\n";
            }
            continue;
            }
            if (args.size() >= 1 && strcmp(args[0].c_str(), "bg") == 0) { // bring to background
                if (args.size() == 2) {
                    if (args[1][0] == '%') {
                    args[1].erase(0, 1); // Remove the '%' character
                    }
                    int jobId = std::stoi(args[1]);
                    // std::cout << args[1] << std::endl;
                    bringToBackground(jobId);
                } else {
                    std::cerr << "Usage: bg <job_id>\n";
                }
                continue;
            }

        if (!args.empty() && args.back() == "&") { // Check if the last argument is "&"
            background = true;
            args.pop_back();

        }
        

        char command_line[MAX_LINE];
        char *argss[MAX_ARGS];
        RedirectionType redir_type;
        char *fileName;
        std::strncpy(command_line, input.c_str(), MAX_LINE);
        
        if(isRedirection){
            parse_command(command_line, argss, &redir_type, &fileName);
            handleRedirectionAndExecute(argss, redir_type, fileName);
        } else if(args.size() >= 1 && strcmp(args[0].c_str(), "echo") == 0){
            echoCommand(args);
        } else if (args.size() >= 1 && strcmp(args[0].c_str(), "jobs") == 0){
            print_jobs();
        }else {
            exitCode = executeCommand(args, background);
        }

        
        exitCode = lastExitCode;
    }
    
    return exitCode;
}