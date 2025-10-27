#include "Utils.hpp"
#include <cctype>
#include <sstream>

std::string toUpperCase(const std::string &str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

std::string toLowerCase(const std::string &str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

bool isValidNickname(const std::string &nick) {
    if (nick.empty() || nick.length() > 9) {
        return false;
    }
    
    if (!std::isalpha(nick[0]) && nick[0] != '_') {
        return false;
    }
    
    for (size_t i = 1; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-') {
            return false;
        }
    }
    
    return true;
}

bool isValidChannel(const std::string &name) {
    if (name.empty() || name.length() < 2 || name.length() > 50) {
        return false;
    }
    
    if (name[0] != '#' && name[0] != '&') {
        return false;
    }
    
    for (size_t i = 1; i < name.length(); ++i) {
        if (name[i] == ' ' || name[i] == ',' || name[i] == ':') {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;
    
    while (std::getline(iss, token, delimiter)) {
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    
    return result;
}
