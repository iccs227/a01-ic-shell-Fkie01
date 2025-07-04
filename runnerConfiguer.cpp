
#include "runnerConfiguer.hpp"
#include "config.hpp"
#include "execute.hpp"
#include "isRedirection.hpp"
#include "jobs.hpp"
#include "parser.hpp"
#include "signalHandle.hpp"
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define MAX_LINE 1024
#define MAX_ARGS 64

int runnerConfig(std::istream *inputStream) {
  std::string input;
  std::string output;
  std::string lastCommand;
  char cwd[PATH_MAX];
  char lastDir[PATH_MAX] = {0};
  int exitCode = 0;
  while (true) {
    bool isRedirection = false;
    bool background = false;
    bool isPipe = false;

    if (inputStream == &std::cin) {
      // std::cout << "icsh $ ";
      // write(STDOUT_FILENO, "icsh $ ", 7);
      // write(STDOUT_FILENO, "\033[1;30;47m", 10); // Bright cyan text on red
      // background write(STDOUT_FILENO, "icsh $ ", 7); write(STDOUT_FILENO,
      // showPrompt, 7);
      showPrompt();
      // write(STDOUT_FILENO, "\033[0m", 4);        // Reset to normal

      std::cout << std::flush;
    }

    if (!std::getline(*inputStream, input)) {
      if (inputStream != &std::cin) {
        break;
      } else {
        std::cin.clear();
        std::cout << std::endl;
        continue;
      }
    }

    // Handle !!
    if (input == "!!") {
      if (lastCommand.empty()) {
        std::cerr << "No previous command to repeat." << std::endl;
        continue;
      }
      std::cout << lastCommand << std::endl;
      input = lastCommand;
    } else {
      lastCommand = input;
    }

    getcwd(cwd, sizeof(cwd));
    // std::cout << "Current directory: " << cwd << std::endl;

    std::vector<std::string> args = parseInput(input);
    if (args.empty()) {
      continue;
    }

    // Handle back command
    if (args[0] == "back") {
      // getcwd(cwd, sizeof(cwd));

      // std::cout << "Going back to previous directory" << std::endl;
      if (lastDir[0] == '\0') {
        // std::cerr << "No previous directory to go back to." << std::endl;
        continue;
      } else {
        // std::cout << "not else" << std::endl;
        if (chdir(lastDir) != 0) {
          perror("chdir");
          continue;
        }
        continue;
      }
      // strncpy(lastDir, cwd, sizeof(lastDir));
      // continue;
    }
    strncpy(lastDir, cwd, sizeof(lastDir));
    // std::cout << "Last directory: " << lastDir << std::endl;

    // Check for redirection
    if (args.size() >= 1) {
      for (size_t i = 1; i < args.size(); ++i) {
        if (std::string(args[i]) == ">" || std::string(args[i]) == "<") {
          isRedirection = true;
          break;
        } else if (strcmp(args[i].c_str(), "|") == 0) {
          isPipe = true;
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
    if (!args.empty() &&
        args.back() == "&") { // Check if the last argument is "&"
      background = true;
      args.pop_back();
      // continue;
    }

    // Handle echo $?
    if (args.size() >= 2 && strcmp(args[0].c_str(), "echo") == 0 &&
        args[1] == "$?") {
      if (lastExitCode != 0) {
        std::cout << lastExitCode << std::endl;
        lastExitCode = 0;
        continue;
      } else {
        std::cout << lastExitCode << std::endl;
        continue;
      }
    }
    // Handle echo command
    if (args.size() >= 1 && strcmp(args[0].c_str(), "echo") == 0) {
      for (size_t i = 1; i < args.size(); ++i) {
        std::cout << args[i] << " ";
      }
      std::cout << std::endl;
      continue;
    }

    std::chrono::high_resolution_clock::time_point start, end;
    if (configEnabled("show_exec_time")) {
      start = std::chrono::high_resolution_clock::now();
    }
    if (args.size() >= 1 &&
        strcmp(args[0].c_str(), "jobs") == 0) { // print jobs
      print_jobs();
      continue;
    }

    if (args.size() >= 1 &&
        strcmp(args[0].c_str(), "fg") == 0) { // bring to foreground
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
    if (args.size() >= 1 &&
        strcmp(args[0].c_str(), "bg") == 0) { // bring to background
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

    // if (!args.empty() && args.back() == "&") { // Check if the last argument
    // is "&"
    //     background = true;
    //     args.pop_back();

    // }

    char command_line[MAX_LINE];
    char *argss[MAX_ARGS];
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];
    RedirectionType redir_type;
    char *fileName;
    std::strncpy(command_line, input.c_str(), MAX_LINE);

    if (isRedirection) {
      parse_command(command_line, argss, &redir_type, &fileName);
      handleRedirectionAndExecute(argss, redir_type, fileName);
    } else if (isPipe) {
      parse_PipeCommand(command_line, args1, args2);
      handlePipeAndExecute(args1, args2);
    } else if (strcmp(args[0].c_str(), "cd") == 0) {
      changeDirectory(args[1]);
      continue;
    } else {
      exitCode = executeCommand(args, background);
      // std::cout << "executeCommand exit code: " <<  std::endl;
    }
    if (configEnabled("show_exec_time")) {
      end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
      std::cout << "\033[1;30;44m"; // Start color
      std::cout << "⏱ Execution time: " << duration << " ms ";
      std::cout << "\033[0m"; // Reset color
      std::cout << std::flush;
    }

    exitCode = lastExitCode;
  }
  return exitCode;
}
