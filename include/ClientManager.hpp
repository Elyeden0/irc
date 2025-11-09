#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include "User.hpp"
#include <map>

class ClientManager {
public:
    ClientManager();
    ~ClientManager();
    
    void addUser(int fd);
    void removeUser(int fd);
    User* getUser(int fd);
    User* getUserByNick(const std::string &nickname);
    
    typedef std::map<int, User*>::iterator iterator;
    iterator begin();
    iterator end();
    
private:
    std::map<int, User*> _users;
};

#endif
