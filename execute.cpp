#include "execute.hpp"
#include "jobs.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int lastExitCode = 0; // Global variable to store the last exit code
extern std::vector<Job> jobs;
int executeCommand(const std::vector<std::string> &args, bool background) {
    // int lastExitCode = 0; // Local variable to store the last exit code
    if (args.empty())
        return 0;

    std::vector<char *> c_args;
    for (const auto &arg : args) {
    c_args.push_back(strdup(arg.c_str()));
    }

    std::vector<std::string> command;
    for(const auto &arg : args){
        command.push_back(arg);
    }

    c_args.push_back(nullptr); // Null-terminate the array
    // command.push_back(nullptr);


    pid_t pid = fork();
    int exitCode = 0;

    if (pid == 0) {
        setpgid(0, 0); // Set the process group ID to the child process ID
        if (execvp(c_args[0], c_args.data()) == -1) { // Execute the command
            // perror("execvp failed");
            std::cerr << "Error executing command: " << c_args[0] << std::endl;
            exit(EXIT_FAILURE);

        }
    } else if (pid > 0) {
        if(background){
            setpgid(pid, pid); // Set the process group ID to the child process ID
            static int jobId = 1;
            std::cout << "[" << jobId << "] " << pid << "   "  << std::endl;
            jobs.push_back({jobId, pid, command, 0,background});
            jobId++;
            lastExitCode = 0;
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            std::cout << status << std::endl;
            if (WIFEXITED(status)) {
                exitCode = WEXITSTATUS(status);
            } else if(WIFSIGNALED(status)) {
                exitCode = WTERMSIG(status);
                // exitCode = 128 + sign;

            } 
            else {
                exitCode = 2; // Non-zero exit code for abnormal termination
            }
            lastExitCode = exitCode;
        }
        
    } else {
        perror("fork failed");
        std::cerr << "Child process terminated abnormally" << std::endl;
    } 
    std::cout << lastExitCode << std::endl;
    return lastExitCode;
}