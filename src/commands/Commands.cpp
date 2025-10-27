#include "Commands.hpp"
#include "Utils.hpp"
#include <map>

CommandHandler getCommandHandler(const std::string &command) {
    static std::map<std::string, CommandHandler> handlers;
    
    if (handlers.empty()) {
        handlers["PASS"] = cmdPass;
        handlers["NICK"] = cmdNick;
        handlers["USER"] = cmdUser;
        handlers["JOIN"] = cmdJoin;
        handlers["PART"] = cmdPart;
        handlers["PRIVMSG"] = cmdPrivmsg;
        handlers["NOTICE"] = cmdNotice;
        handlers["QUIT"] = cmdQuit;
        handlers["PING"] = cmdPing;
        handlers["KICK"] = cmdKick;
        handlers["TOPIC"] = cmdTopic;
        handlers["INVITE"] = cmdInvite;
        handlers["MODE"] = cmdMode;
        handlers["OPER"] = cmdOper;
    }
    
    std::string upper = toUpperCase(command);
    std::map<std::string, CommandHandler>::iterator it = handlers.find(upper);
    return (it != handlers.end()) ? it->second : NULL;
}
