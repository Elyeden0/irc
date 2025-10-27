#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include "Room.hpp"
#include "User.hpp"
#include <map>
#include <vector>
#include <string>

class ChannelManager {
public:
    ChannelManager();
    ~ChannelManager();
    
    Room* getRoom(const std::string &name);
    Room* createRoom(const std::string &name, User *creator);
    void removeRoom(const std::string &name);
    
    std::vector<Room*> getUserRooms(User *user) const;
    void removeUserFromAll(User *user);
    
private:
    std::map<std::string, Room*> _rooms;
};

#endif
