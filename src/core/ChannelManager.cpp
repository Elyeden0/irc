#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {}

ChannelManager::~ChannelManager() {
    for (std::map<std::string, Room*>::iterator it = _rooms.begin(); it != _rooms.end(); ++it) {
        delete it->second;
    }
}

// Room* ChannelManager::getRoom(const std::string &name) {
//     std::map<std::string, Room*>::iterator it = _rooms.find(name);
//     return (it != _rooms.end()) ? it->second : NULL;
// }

// Room* ChannelManager::createRoom(const std::string &name, User *creator) {
//     Room *room = getRoom(name);
//     if (!room) {
//         room = new Room(name);
//         _rooms[name] = room;
//         room->addMember(creator);
//         room->addOperator(creator);
//     }
//     return room;
// // }

// // void ChannelManager::removeRoom(const std::string &name) {
// //     std::map<std::string, Room*>::iterator it = _rooms.find(name);
// //     if (it != _rooms.end()) {
// //         delete it->second;
// //         _rooms.erase(it);
// //     }
// // }

// std::vector<Room*> ChannelManager::getUserRooms(User *user) const {
//     std::vector<Room*> rooms;
//     for (std::map<std::string, Room*>::const_iterator it = _rooms.begin(); it != _rooms.end(); ++it) {
//         if (it->second->hasMember(user)) {
//             rooms.push_back(it->second);
//         }
//     }
//     return rooms;
// // }

// // void ChannelManager::removeUserFromAll(User *user) {
// //     std::vector<std::string> toRemove;
    
// //     for (std::map<std::string, Room*>::iterator it = _rooms.begin(); it != _rooms.end(); ++it) {
// //         if (it->second->hasMember(user)) {
// //             it->second->removeMember(user);
// //             if (it->second->isEmpty()) {
// //                 toRemove.push_back(it->first);
// //             }
// //         }
// //     }
    
// //     for (size_t i = 0; i < toRemove.size(); ++i) {
// //         removeRoom(toRemove[i]);
// //     }
// // }
