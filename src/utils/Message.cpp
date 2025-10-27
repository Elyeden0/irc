#include "Message.hpp"
#include "Utils.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>

static std::string parseToken(std::istringstream &iss) {
    std::string token;
    std::getline(iss, token, ' ');
    return token;
}

Message parseMessage(const std::string &raw) {
    Message msg;
    std::istringstream iss(raw);
    
    if (raw.empty()) {
        throw std::invalid_argument("Empty message");
    }
    
    if (raw[0] == ':') {
        iss.ignore(1);
        msg.prefix = parseToken(iss);
    }
    
    msg.command = parseToken(iss);
    if (msg.command.empty()) {
        throw std::invalid_argument("No command");
    }
    
    msg.command = toUpperCase(msg.command);
    
    while (!iss.eof()) {
        if (iss.peek() == ':') {
            iss.ignore(1);
            std::getline(iss, msg.trailing);
            break;
        }
        std::string param = parseToken(iss);
        if (!param.empty()) {
            msg.params.push_back(param);
        }
    }
    
    return msg;
}
