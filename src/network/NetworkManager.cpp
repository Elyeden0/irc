#include "NetworkManager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <sstream>

NetworkManager::NetworkManager() : _serverFd(-1) {}

NetworkManager::~NetworkManager() {
    if (_serverFd != -1) {
        close(_serverFd);
    }
    for (size_t i = 0; i < _fds.size(); ++i) {
        if (_fds[i].fd != _serverFd) {
            close(_fds[i].fd);
        }
    }
}

bool NetworkManager::initSocket(int port) {
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1) {
        std::cerr << "socket() failed" << std::endl;
        return false;
    }
    
    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "bind() failed" << std::endl;
        close(_serverFd);
        return false;
    }
    
    if (listen(_serverFd, 10) == -1) {
        std::cerr << "listen() failed" << std::endl;
        close(_serverFd);
        return false;
    }
    
    struct pollfd pfd;
    pfd.fd = _serverFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
    
    return true;
}

int NetworkManager::acceptConnection() {
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &len);
    
    if (clientFd != -1) {
        fcntl(clientFd, F_SETFL, O_NONBLOCK);
        
        char addrStr[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &clientAddr.sin_addr, addrStr, INET_ADDRSTRLEN)) {
            std::cout << "Connection from " << addrStr 
                      << ":" << ntohs(clientAddr.sin_port) << std::endl;
        }
    }
    
    return clientFd;
}

int NetworkManager::pollEvents() {
    return poll(&_fds[0], _fds.size(), -1);
}

void NetworkManager::addClient(int fd) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
}

void NetworkManager::removeClient(int fd) {
    for (size_t i = 1; i < _fds.size(); ++i) {
        if (_fds[i].fd == fd) {
            close(fd);
            _fds.erase(_fds.begin() + i);
            break;
        }
    }
}

bool NetworkManager::hasNewConnection() const {
    return (_fds[0].revents & POLLIN);
}

std::vector<int> NetworkManager::getReadyClients() const {
    std::vector<int> ready;
    for (size_t i = 1; i < _fds.size(); ++i) {
        if (_fds[i].revents & POLLIN) {
            ready.push_back(_fds[i].fd);
        }
    }
    return ready;
}

std::string NetworkManager::getClientAddress(int fd) const {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    
    if (getpeername(fd, (struct sockaddr*)&addr, &len) == -1) {
        return "unknown";
    }
    
    return getAddressString(&addr);
}

std::string NetworkManager::getAddressString(struct sockaddr_in *addr) const {
    char addrStr[INET_ADDRSTRLEN];
    
    if (!inet_ntop(AF_INET, &addr->sin_addr, addrStr, INET_ADDRSTRLEN)) {
        return "unknown";
    }
    
    std::ostringstream oss;
    oss << addrStr << ":" << ntohs(addr->sin_port);
    return oss.str();
}
