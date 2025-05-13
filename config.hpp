#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <map>

void loadConfig();
bool configEnabled(const std::string& key);
void showPrompt();
std::string getCurrentTime();
std::string getGitBranch();
void changeDirectory(const std::string& path);

#endif // CONFIG_HPP