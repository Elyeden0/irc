#include "Commands.hpp"
#include "Replies.hpp"
#include "Utils.hpp"
#include <sstream>

void cmdKick(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.size() < 2) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "KICK"));
        return;
    }
    
    std::string chanName = msg.params[0];
    std::string targetNick = msg.params[1];
    std::string reason = msg.trailing.empty() ? user->getNickname() : msg.trailing;
    
    Room *room = server.getChannels().getRoom(chanName);
    if (!room) {
        user->send(ERR_NOSUCHCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (!room->hasMember(user)) {
        user->send(ERR_NOTONCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (!room->isOperator(user)) {
        user->send(ERR_CHANOPRIVSNEEDED(user->getNickname(), chanName));
        return;
    }
    
    User *target = room->getMember(targetNick);
    if (!target) {
        user->send(ERR_USERNOTINCHANNEL(user->getNickname(), targetNick, chanName));
        return;
    }
    
    room->broadcast(MSG_KICK(user->getPrefix(), chanName, targetNick, reason));
    room->removeMember(target);
    
    if (room->isEmpty()) {
        server.getChannels().removeRoom(chanName);
    }
}

void cmdMode(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.empty()) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "MODE"));
        return;
    }
    
    std::string target = msg.params[0];
    
    if (!isValidChannel(target)) {
        return;
    }
    
    Room *room = server.getChannels().getRoom(target);
    if (!room) {
        user->send(ERR_NOSUCHCHANNEL(user->getNickname(), target));
        return;
    }
    
    if (!room->hasMember(user)) {
        user->send(ERR_NOTONCHANNEL(user->getNickname(), target));
        return;
    }
    
    if (msg.params.size() == 1) {
        user->send(RPL_CHANNELMODEIS(user->getNickname(), target, room->getModeString()));
        return;
    }
    
    if (!room->isOperator(user)) {
        user->send(ERR_CHANOPRIVSNEEDED(user->getNickname(), target));
        return;
    }
    
    std::string modes = msg.params[1];
    bool adding = true;
    size_t paramIndex = 2;
    std::string appliedModes;
    std::string modeParams;
    
    for (size_t i = 0; i < modes.length(); ++i) {
        char mode = modes[i];
        
        if (mode == '+') {
            adding = true;
            continue;
        }
        if (mode == '-') {
            adding = false;
            continue;
        }
        
        if (mode == 'i') {
            room->setMode(MODE_INVITE_ONLY, adding);
            appliedModes += mode;
        } else if (mode == 't') {
            room->setMode(MODE_TOPIC_RESTRICTED, adding);
            appliedModes += mode;
        } else if (mode == 'k') {
            if (adding && paramIndex < msg.params.size()) {
                room->setKey(msg.params[paramIndex]);
                appliedModes += mode;
                modeParams += " " + msg.params[paramIndex];
                paramIndex++;
            } else if (!adding) {
                room->setKey("");
                appliedModes += mode;
            }
        } else if (mode == 'l') {
            if (adding && paramIndex < msg.params.size()) {
                std::istringstream iss(msg.params[paramIndex]);
                unsigned int limit;
                if (iss >> limit) {
                    room->setLimit(limit);
                    appliedModes += mode;
                    modeParams += " " + msg.params[paramIndex];
                    paramIndex++;
                }
            } else if (!adding) {
                room->setLimit(0);
                appliedModes += mode;
            }
        } else if (mode == 'o') {
            if (paramIndex < msg.params.size()) {
                User *targetUser = room->getMember(msg.params[paramIndex]);
                if (targetUser) {
                    if (adding) {
                        room->addOperator(targetUser);
                    } else {
                        room->removeOperator(targetUser);
                    }
                    appliedModes += mode;
                    modeParams += " " + msg.params[paramIndex];
                }
                paramIndex++;
            }
        } else {
            user->send(ERR_UNKNOWNMODE(user->getNickname(), mode));
        }
    }
    
    if (!appliedModes.empty()) {
        std::string modeChange = std::string(adding ? "+" : "-") + appliedModes + modeParams;
        room->broadcast(MSG_MODE(user->getPrefix(), target, modeChange));
    }
}

void cmdOper(IRCServer &server, User *user, Message &msg) {
    (void)server;
    
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.size() < 2) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "OPER"));
        return;
    }
    
    std::string name = msg.params[0];
    std::string password = msg.params[1];
    
    if (name != "admin" || password != "admin") {
        user->send(ERR_NOOPERHOST(user->getNickname()));
        return;
    }
    
    user->setOperator(true);
    user->send(RPL_YOUREOPER(user->getNickname()));
}
