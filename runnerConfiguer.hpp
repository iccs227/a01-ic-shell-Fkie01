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

int runnerConfig(std::istream *inputStream);

#endif // RUNNER_CONFIGUER_HPP