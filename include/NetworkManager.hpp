#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <vector>
#include <poll.h>
#include <string>

const int error = -1;
const int nothing = 0;
const int client = 1;
const int message = 2;
const int deconexion = 3;


class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    bool initSocket(int port);
    int acceptConnection();
    int pollEvents();
    
    void addClient(int fd);
    int make_poll();
    void removeClient(int fd);
    bool hasNewConnection() const;
    void	error_text(std::string msg);
    
    std::vector<int> getReadyClients() const;
    
    std::string getClientAddress(int fd) const;
    
private:
    int _serverFd;
    std::vector<struct pollfd> _fds;
    std::string getAddressString(struct sockaddr_in *addr) const;
};

#endif
