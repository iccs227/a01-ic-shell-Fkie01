/* ICCS227: Project 1: icsh
 * Name: Subhakorn Chayawatto 
 * StudentID: 6680597
 */

#include "parser.hpp"
#include "config.hpp"
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
#include <stdlib.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(int argc, char *argv[]) {
    setupSignalHandlers(); // handle ctrl-c and ctrl-z
    loadConfig(); // load config from ~/.myshrc
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

    
    


    write(STDOUT_FILENO, "  -------------welcome to icsh shell by me ;_; <--------------\n", 66);

    exitCode = runnerConfig(inputStream);
    
    
    return exitCode;
}