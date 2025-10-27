#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "NetworkManager.hpp"
#include "ClientManager.hpp"
#include "ChannelManager.hpp"
#include <string>

class IRCServer {
public:
    IRCServer(int port, const std::string &password);
    ~IRCServer();
    
    bool start();
    void stop();
    
    const std::string& getPassword() const;
    ClientManager& getClients();
    ChannelManager& getChannels();
    
    static void signalHandler(int signal);
    
private:
    NetworkManager _network;
    ClientManager _clients;
    ChannelManager _channels;
    const std::string _password;
    static bool _running;
};

#endif
