#ifndef ROOM_HPP
#define ROOM_HPP

#include "User.hpp"
#include <string>
#include <vector>
#include <set>

enum RoomMode {
    MODE_NONE = 0,
    MODE_TOPIC_RESTRICTED = 1 << 0,
    MODE_INVITE_ONLY = 1 << 1
};

class Room {
public:
    Room(const std::string &name);
    ~Room();
    
    const std::string& getName() const;
    const std::string& getTopic() const;
    void setTopic(const std::string &topic);
    
    void setKey(const std::string &key);
    bool checkKey(const std::string &key) const;
    bool hasKey() const;
    
    void addMember(User *user);
    void removeMember(User *user);
    bool hasMember(User *user) const;
    User* getMember(const std::string &nick) const;
    
    void addOperator(User *user);
    void removeOperator(User *user);
    bool isOperator(User *user) const;
    
    void addInvited(User *user);
    void removeInvited(User *user);
    bool isInvited(User *user) const;
    
    void broadcast(const std::string &msg) const;
    void broadcastExcept(User *user, const std::string &msg) const;
    
    void setMode(RoomMode mode, bool enable);
    bool hasMode(RoomMode mode) const;
    std::string getModeString() const;
    
    void setLimit(unsigned int limit);
    unsigned int getLimit() const;
    bool isFull() const;
    
    unsigned int getMemberCount() const;
    std::string getMemberList() const;
    
    bool isEmpty() const;
    
private:
    std::string _name;
    std::string _topic;
    std::string _key;
    int _modes;
    unsigned int _limit;
    
    std::vector<User*> _members;
    std::set<User*> _operators;
    std::set<User*> _invited;
};

#endif
