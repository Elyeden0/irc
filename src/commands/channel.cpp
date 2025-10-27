#include "Commands.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

void cmdJoin(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.empty() && msg.trailing.empty()) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "JOIN"));
        return;
    }
    
    std::string channels = msg.params.empty() ? msg.trailing : msg.params[0];
    std::string keys = msg.params.size() > 1 ? msg.params[1] : "";
    
    std::vector<std::string> channelList = splitString(channels, ',');
    std::vector<std::string> keyList = splitString(keys, ',');
    
    for (size_t i = 0; i < channelList.size(); ++i) {
        std::string chanName = channelList[i];
        std::string key = i < keyList.size() ? keyList[i] : "";
        
        if (!isValidChannel(chanName)) {
            user->send(ERR_NOSUCHCHANNEL(user->getNickname(), chanName));
            continue;
        }
        
        Room *room = server.getChannels().getRoom(chanName);
        
        if (!room) {
            room = server.getChannels().createRoom(chanName, user);
            user->send(MSG_JOIN(user->getPrefix(), chanName));
            
            if (!room->getTopic().empty()) {
                user->send(RPL_TOPIC(user->getNickname(), chanName, room->getTopic()));
            } else {
                user->send(RPL_NOTOPIC(user->getNickname(), chanName));
            }
            
            user->send(RPL_NAMREPLY(user->getNickname(), chanName, room->getMemberList()));
            user->send(RPL_ENDOFNAMES(user->getNickname(), chanName));
            continue;
        }
        
        if (room->hasMember(user)) {
            continue;
        }
        
        if (room->hasMode(MODE_INVITE_ONLY) && !room->isInvited(user)) {
            user->send(ERR_INVITEONLYCHAN(user->getNickname(), chanName));
            continue;
        }
        
        if (room->isFull()) {
            user->send(ERR_CHANNELISFULL(user->getNickname(), chanName));
            continue;
        }
        
        if (!room->checkKey(key)) {
            user->send(ERR_BADCHANNELKEY(user->getNickname(), chanName));
            continue;
        }
        
        room->addMember(user);
        room->removeInvited(user);
        
        room->broadcast(MSG_JOIN(user->getPrefix(), chanName));
        
        if (!room->getTopic().empty()) {
            user->send(RPL_TOPIC(user->getNickname(), chanName, room->getTopic()));
        } else {
            user->send(RPL_NOTOPIC(user->getNickname(), chanName));
        }
        
        user->send(RPL_NAMREPLY(user->getNickname(), chanName, room->getMemberList()));
        user->send(RPL_ENDOFNAMES(user->getNickname(), chanName));
    }
}

void cmdPart(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.empty() && msg.trailing.empty()) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "PART"));
        return;
    }
    
    std::string channels = msg.params.empty() ? msg.trailing : msg.params[0];
    std::vector<std::string> channelList = splitString(channels, ',');
    
    for (size_t i = 0; i < channelList.size(); ++i) {
        std::string chanName = channelList[i];
        Room *room = server.getChannels().getRoom(chanName);
        
        if (!room) {
            user->send(ERR_NOSUCHCHANNEL(user->getNickname(), chanName));
            continue;
        }
        
        if (!room->hasMember(user)) {
            user->send(ERR_NOTONCHANNEL(user->getNickname(), chanName));
            continue;
        }
        
        room->broadcast(MSG_PART(user->getPrefix(), chanName));
        room->removeMember(user);
        
        if (room->isEmpty()) {
            server.getChannels().removeRoom(chanName);
        }
    }
}

void cmdTopic(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.empty()) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "TOPIC"));
        return;
    }
    
    std::string chanName = msg.params[0];
    Room *room = server.getChannels().getRoom(chanName);
    
    if (!room) {
        user->send(ERR_NOSUCHCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (!room->hasMember(user)) {
        user->send(ERR_NOTONCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (msg.trailing.empty() && msg.params.size() == 1) {
        if (room->getTopic().empty()) {
            user->send(RPL_NOTOPIC(user->getNickname(), chanName));
        } else {
            user->send(RPL_TOPIC(user->getNickname(), chanName, room->getTopic()));
        }
        return;
    }
    
    if (room->hasMode(MODE_TOPIC_RESTRICTED) && !room->isOperator(user)) {
        user->send(ERR_CHANOPRIVSNEEDED(user->getNickname(), chanName));
        return;
    }
    
    room->setTopic(msg.trailing);
    room->broadcast(MSG_TOPIC(user->getPrefix(), chanName, msg.trailing));
}

void cmdInvite(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.size() < 2) {
        user->send(ERR_NEEDMOREPARAMS(user->getNickname(), "INVITE"));
        return;
    }
    
    std::string targetNick = msg.params[0];
    std::string chanName = msg.params[1];
    
    User *target = server.getClients().getUserByNick(targetNick);
    if (!target) {
        user->send(ERR_NOSUCHNICK(user->getNickname(), targetNick));
        return;
    }
    
    Room *room = server.getChannels().getRoom(chanName);
    if (!room) {
        user->send(ERR_NOSUCHCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (!room->hasMember(user)) {
        user->send(ERR_NOTONCHANNEL(user->getNickname(), chanName));
        return;
    }
    
    if (room->hasMember(target)) {
        user->send(ERR_USERONCHANNEL(user->getNickname(), targetNick, chanName));
        return;
    }
    
    if (room->hasMode(MODE_INVITE_ONLY) && !room->isOperator(user)) {
        user->send(ERR_CHANOPRIVSNEEDED(user->getNickname(), chanName));
        return;
    }
    
    room->addInvited(target);
    user->send(RPL_INVITING(user->getNickname(), targetNick, chanName));
    target->send(MSG_INVITE(user->getPrefix(), targetNick, chanName));
}
