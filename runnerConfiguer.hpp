#ifndef RUNNER_CONFIGUER_HPP
#define RUNNER_CONFIGUER_HPP

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

std::string checkDoubleBang(std::string &input, std::string &lastCommand);
bool checkIfRedirection(std::vector<std::string> &args);
void echoCommand(std::vector<std::string> &args);
void printTheJob(std::vector<std::string> &args);

#endif // RUNNER_CONFIGUER_HPP