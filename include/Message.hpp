#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

#define CRLF "\r\n"

struct Message {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
};

Message parseMessage(const std::string &raw);

#endif
