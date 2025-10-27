#include "Commands.hpp"
#include "Replies.hpp"
#include "Utils.hpp"
#include <arpa/inet.h>

void cmdPass(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() >= AUTH_PASS) {
        user->send(ERR_ALREADYREGISTRED(user->getNickname()));
        return;
    }
    
    if (msg.params.empty() && msg.trailing.empty()) {
        user->send(ERR_NEEDMOREPARAMS("*", "PASS"));
        return;
    }
    
    std::string password = msg.params.empty() ? msg.trailing : msg.params[0];
    
    if (password != server.getPassword()) {
        user->send(ERR_PASSWDMISMATCH("*"));
        return;
    }
    
    user->setAuthState(AUTH_PASS);
}

void cmdNick(IRCServer &server, User *user, Message &msg) {
    if (msg.params.empty() && msg.trailing.empty()) {
        std::string nick = user->getAuthState() >= AUTH_NICK ? user->getNickname() : "*";
        user->send(ERR_NONICKNAMEGIVEN(nick));
        return;
    }
    
    std::string newNick = msg.params.empty() ? msg.trailing : msg.params[0];
    
    if (!isValidNickname(newNick)) {
        std::string nick = user->getAuthState() >= AUTH_NICK ? user->getNickname() : "*";
        user->send(ERR_ERRONEUSNICKNAME(nick, newNick));
        return;
    }
    
    if (server.getClients().getUserByNick(newNick)) {
        std::string nick = user->getAuthState() >= AUTH_NICK ? user->getNickname() : "*";
        user->send(ERR_NICKNAMEINUSE(nick, newNick));
        return;
    }
    
    if (user->getAuthState() == AUTH_DONE) {
        std::string oldPrefix = user->getPrefix();
        user->setNickname(newNick);
        
        std::vector<Room*> rooms = server.getChannels().getUserRooms(user);
        for (size_t i = 0; i < rooms.size(); ++i) {
            rooms[i]->broadcast(MSG_NICK(oldPrefix, newNick));
        }
    } else {
        user->setNickname(newNick);
        if (user->getAuthState() < AUTH_NICK) {
            user->setAuthState(AUTH_NICK);
        }
    }
}

void cmdUser(IRCServer &server, User *user, Message &msg) {
    (void)server;
    
    if (user->getAuthState() == AUTH_DONE) {
        user->send(ERR_ALREADYREGISTRED(user->getNickname()));
        return;
    }
    
    if (msg.params.size() < 3 || msg.trailing.empty()) {
        user->send(ERR_NEEDMOREPARAMS("*", "USER"));
        return;
    }
    
    if (user->getAuthState() != AUTH_NICK) {
        return;
    }
    
    user->setUsername(msg.params[0]);
    user->setHostname(msg.params[1]);
    user->setRealname(msg.trailing);
    user->setAuthState(AUTH_DONE);
    
    user->send(RPL_WELCOME(user->getNickname(), user->getUsername(), user->getHostname()));
    user->send(RPL_YOURHOST(user->getNickname()));
    user->send(RPL_CREATED(user->getNickname()));
    user->send(RPL_MYINFO(user->getNickname()));
}
