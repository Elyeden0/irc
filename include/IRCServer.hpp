#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include "NetworkManager.hpp"
#include "ClientManager.hpp"
#include "ChannelManager.hpp"
#include <string>

const int error = -1;
const int nothing = 0;
const int client = 1;
const int message = 2;
const int deconexion = 3;

class IRCServer {
public:
    IRCServer(int port, const std::string &password);
    ~IRCServer();
    
    bool start();
    int léa(int port);
    void stop();
    
    const std::string& getPassword() const;
    ClientManager& getClients();
    ChannelManager& getChannels();
    
    static void signalHandler(int signal);

    int     create_network(int port);
    int     new_client();
    int	poll_checker();
    void	error_text(std::string msg);
    
private:
    NetworkManager _network;
    ClientManager _clients;
    ChannelManager _channels;
    const std::string _password;
    static bool _running;
    int _server_socket;
};

#endif
