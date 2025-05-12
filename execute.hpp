#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include <string>
#include <vector>

extern int lastExitCode;

int executeCommand(const std::vector<std::string> &args, bool isBackground);
#endif // EXECUTE_HPP