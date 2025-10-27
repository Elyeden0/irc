#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>

#define REPLY(num, target, msg) (":" + std::string("IRC.server") + " " + num + " " + target + " " + msg + CRLF)

#define RPL_WELCOME(nick, user, host) REPLY("001", nick, ":Welcome to IRC " + nick + "!" + user + "@" + host)
#define RPL_YOURHOST(nick) REPLY("002", nick, ":Your host is IRC.server")
#define RPL_CREATED(nick) REPLY("003", nick, ":This server was created")
#define RPL_MYINFO(nick) REPLY("004", nick, "IRC.server 1.0 o oitkl")

#define RPL_NOTOPIC(nick, chan) REPLY("331", nick, chan + " :No topic is set")
#define RPL_TOPIC(nick, chan, topic) REPLY("332", nick, chan + " :" + topic)
#define RPL_NAMREPLY(nick, chan, names) REPLY("353", nick, "= " + chan + " :" + names)
#define RPL_ENDOFNAMES(nick, chan) REPLY("366", nick, chan + " :End of NAMES list")
#define RPL_CHANNELMODEIS(nick, chan, modes) REPLY("324", nick, chan + " +" + modes)
#define RPL_YOUREOPER(nick) REPLY("381", nick, ":You are now an IRC operator")
#define RPL_INVITING(nick, target, chan) REPLY("341", nick, target + " " + chan)

#define ERR_NOSUCHNICK(nick, target) REPLY("401", nick, target + " :No such nick")
#define ERR_NOSUCHCHANNEL(nick, chan) REPLY("403", nick, chan + " :No such channel")
#define ERR_NORECIPIENT(nick, cmd) REPLY("411", nick, cmd + " :No recipient")
#define ERR_NOTEXTTOSEND(nick) REPLY("412", nick, ":No text to send")
#define ERR_UNKNOWNCOMMAND(nick, cmd) REPLY("421", nick, cmd + " :Unknown command")
#define ERR_NONICKNAMEGIVEN(nick) REPLY("431", nick, ":No nickname given")
#define ERR_ERRONEUSNICKNAME(nick, bad) REPLY("432", nick, bad + " :Erroneous nickname")
#define ERR_NICKNAMEINUSE(nick, bad) REPLY("433", nick, bad + " :Nickname in use")
#define ERR_USERNOTINCHANNEL(nick, target, chan) REPLY("441", nick, target + " " + chan + " :They aren't on that channel")
#define ERR_NOTONCHANNEL(nick, chan) REPLY("442", nick, chan + " :You're not on that channel")
#define ERR_USERONCHANNEL(nick, target, chan) REPLY("443", nick, target + " " + chan + " :is already on channel")
#define ERR_NOTREGISTERED(nick) REPLY("451", nick, ":You have not registered")
#define ERR_NEEDMOREPARAMS(nick, cmd) REPLY("461", nick, cmd + " :Not enough parameters")
#define ERR_ALREADYREGISTRED(nick) REPLY("462", nick, ":You may not reregister")
#define ERR_PASSWDMISMATCH(nick) REPLY("464", nick, ":Password incorrect")
#define ERR_CHANNELISFULL(nick, chan) REPLY("471", nick, chan + " :Cannot join channel (+l)")
#define ERR_UNKNOWNMODE(nick, mode) REPLY("472", nick, std::string(1, mode) + " :Unknown mode")
#define ERR_INVITEONLYCHAN(nick, chan) REPLY("473", nick, chan + " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(nick, chan) REPLY("475", nick, chan + " :Cannot join channel (+k)")
#define ERR_CHANOPRIVSNEEDED(nick, chan) REPLY("482", nick, chan + " :You're not channel operator")
#define ERR_NOOPERHOST(nick) REPLY("491", nick, ":No O-lines for your host")

#define MSG_PONG "PONG IRC.server" CRLF
#define MSG_JOIN(prefix, chan) (":" + prefix + " JOIN " + chan + CRLF)
#define MSG_PART(prefix, chan) (":" + prefix + " PART " + chan + CRLF)
#define MSG_QUIT(prefix, reason) (":" + prefix + " QUIT :" + reason + CRLF)
#define MSG_KICK(prefix, chan, target, reason) (":" + prefix + " KICK " + chan + " " + target + " :" + reason + CRLF)
#define MSG_PRIVMSG(prefix, target, text) (":" + prefix + " PRIVMSG " + target + " :" + text + CRLF)
#define MSG_NOTICE(prefix, target, text) (":" + prefix + " NOTICE " + target + " :" + text + CRLF)
#define MSG_TOPIC(prefix, chan, topic) (":" + prefix + " TOPIC " + chan + " :" + topic + CRLF)
#define MSG_INVITE(prefix, nick, chan) (":" + prefix + " INVITE " + nick + " " + chan + CRLF)
#define MSG_MODE(prefix, target, modes) (":" + prefix + " MODE " + target + " " + modes + CRLF)
#define MSG_NICK(prefix, newnick) (":" + prefix + " NICK " + newnick + CRLF)

#endif
