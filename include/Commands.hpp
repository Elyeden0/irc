#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "IRCServer.hpp"
#include "User.hpp"
#include "Message.hpp"

typedef void (*CommandHandler)(IRCServer&, User*, Message&);

CommandHandler getCommandHandler(const std::string &command);

void cmdPass(IRCServer &server, User *user, Message &msg);
void cmdNick(IRCServer &server, User *user, Message &msg);
void cmdUser(IRCServer &server, User *user, Message &msg);
void cmdJoin(IRCServer &server, User *user, Message &msg);
void cmdPart(IRCServer &server, User *user, Message &msg);
void cmdPrivmsg(IRCServer &server, User *user, Message &msg);
void cmdNotice(IRCServer &server, User *user, Message &msg);
void cmdQuit(IRCServer &server, User *user, Message &msg);
void cmdPing(IRCServer &server, User *user, Message &msg);
void cmdKick(IRCServer &server, User *user, Message &msg);
void cmdTopic(IRCServer &server, User *user, Message &msg);
void cmdInvite(IRCServer &server, User *user, Message &msg);
void cmdMode(IRCServer &server, User *user, Message &msg);
void cmdOper(IRCServer &server, User *user, Message &msg);

#endif
