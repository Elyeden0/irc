#include "Commands.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

void cmdPrivmsg(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        user->send(ERR_NOTREGISTERED("*"));
        return;
    }
    
    if (msg.params.empty()) {
        user->send(ERR_NORECIPIENT(user->getNickname(), "PRIVMSG"));
        return;
    }
    
    if (msg.trailing.empty()) {
        user->send(ERR_NOTEXTTOSEND(user->getNickname()));
        return;
    }
    
    std::string target = msg.params[0];
    
    if (isValidChannel(target)) {
        Room *room = server.getChannels().getRoom(target);
        if (!room) {
            user->send(ERR_NOSUCHCHANNEL(user->getNickname(), target));
            return;
        }
        
        if (!room->hasMember(user)) {
            user->send(ERR_NOTONCHANNEL(user->getNickname(), target));
            return;
        }
        
        room->broadcastExcept(user, MSG_PRIVMSG(user->getPrefix(), target, msg.trailing));
    } else {
        User *targetUser = server.getClients().getUserByNick(target);
        if (!targetUser) {
            user->send(ERR_NOSUCHNICK(user->getNickname(), target));
            return;
        }
        
        targetUser->send(MSG_PRIVMSG(user->getPrefix(), target, msg.trailing));
    }
}

void cmdNotice(IRCServer &server, User *user, Message &msg) {
    if (user->getAuthState() != AUTH_DONE) {
        return;
    }
    
    if (msg.params.empty() || msg.trailing.empty()) {
        return;
    }
    
    std::string target = msg.params[0];
    
    if (isValidChannel(target)) {
        Room *room = server.getChannels().getRoom(target);
        if (!room || !room->hasMember(user)) {
            return;
        }
        
        room->broadcastExcept(user, MSG_NOTICE(user->getPrefix(), target, msg.trailing));
    } else {
        User *targetUser = server.getClients().getUserByNick(target);
        if (!targetUser) {
            return;
        }
        
        targetUser->send(MSG_NOTICE(user->getPrefix(), target, msg.trailing));
    }
}

void cmdQuit(IRCServer &server, User *user, Message &msg) {
    std::string reason = msg.trailing.empty() ? "Client quit" : msg.trailing;
    
    std::vector<Room*> rooms = server.getChannels().getUserRooms(user);
    for (size_t i = 0; i < rooms.size(); ++i) {
        rooms[i]->broadcastExcept(user, MSG_QUIT(user->getPrefix(), reason));
    }
    
    user->markQuit();
}

void cmdPing(IRCServer &server, User *user, Message &msg) {
    (void)server;
    (void)msg;
    user->send(MSG_PONG);
}
