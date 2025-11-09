#include "NetworkManager.hpp"
#include "IRCServer.hpp"

NetworkManager::NetworkManager() : _serverFd(-1) {}

NetworkManager::~NetworkManager() {
    if (_serverFd != -1) {
        close(_serverFd);
    }
}
