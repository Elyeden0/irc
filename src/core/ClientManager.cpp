#include "ClientManager.hpp"

ClientManager::ClientManager() {}

ClientManager::~ClientManager() {
    for (iterator it = _users.begin(); it != _users.end(); ++it) {
        delete it->second;
    }
}

// void ClientManager::addUser(int fd) {
//     _users[fd] = new User(fd);
// }

// void ClientManager::removeUser(int fd) {
//     iterator it = _users.find(fd);
//     if (it != _users.end()) {
//         delete it->second;
//         _users.erase(it);
//     }
// }

User* ClientManager::getUser(int fd) {
    iterator it = _users.find(fd);
    return (it != _users.end()) ? it->second : NULL;
}

User* ClientManager::getUserByNick(const std::string &nickname) {
    for (iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

ClientManager::iterator ClientManager::begin() { return _users.begin(); }
ClientManager::iterator ClientManager::end() { return _users.end(); }
