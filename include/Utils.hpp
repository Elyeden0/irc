#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

std::string toUpperCase(const std::string &str);
std::string toLowerCase(const std::string &str);
bool isValidNickname(const std::string &nick);
bool isValidChannel(const std::string &name);
std::vector<std::string> splitString(const std::string &str, char delimiter);

#endif
