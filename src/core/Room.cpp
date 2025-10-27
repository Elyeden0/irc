#include "Room.hpp"
#include <algorithm>
#include <sstream>

Room::Room(const std::string &name)
    : _name(name), _modes(MODE_NONE), _limit(0) {}

Room::~Room() {}

const std::string& Room::getName() const { return _name; }

const std::string& Room::getTopic() const { return _topic; }

void Room::setTopic(const std::string &topic) { _topic = topic; }

void Room::setKey(const std::string &key) { _key = key; }

bool Room::checkKey(const std::string &key) const {
    return _key.empty() || _key == key;
}

bool Room::hasKey() const {
    return !_key.empty();
}

void Room::addMember(User *user) {
    if (!hasMember(user)) {
        _members.push_back(user);
    }
}

void Room::removeMember(User *user) {
    for (std::vector<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it == user) {
            _members.erase(it);
            break;
        }
    }
    _operators.erase(user);
    _invited.erase(user);
}

bool Room::hasMember(User *user) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i] == user) {
            return true;
        }
    }
    return false;
}

User* Room::getMember(const std::string &nick) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i]->getNickname() == nick) {
            return _members[i];
        }
    }
    return NULL;
}

void Room::addOperator(User *user) {
    _operators.insert(user);
}

void Room::removeOperator(User *user) {
    _operators.erase(user);
}

bool Room::isOperator(User *user) const {
    return _operators.find(user) != _operators.end();
}

void Room::addInvited(User *user) {
    _invited.insert(user);
}

void Room::removeInvited(User *user) {
    _invited.erase(user);
}

bool Room::isInvited(User *user) const {
    return _invited.find(user) != _invited.end();
}

void Room::broadcast(const std::string &msg) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        _members[i]->send(msg);
    }
}

void Room::broadcastExcept(User *user, const std::string &msg) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i] != user) {
            _members[i]->send(msg);
        }
    }
}

void Room::setMode(RoomMode mode, bool enable) {
    if (enable) {
        _modes |= mode;
    } else {
        _modes &= ~mode;
    }
}

bool Room::hasMode(RoomMode mode) const {
    return (_modes & mode) != 0;
}

std::string Room::getModeString() const {
    std::string modes;
    if (hasMode(MODE_INVITE_ONLY)) modes += "i";
    if (hasMode(MODE_TOPIC_RESTRICTED)) modes += "t";
    if (hasKey()) modes += "k";
    if (_limit > 0) modes += "l";
    return modes;
}

void Room::setLimit(unsigned int limit) {
    _limit = limit;
}

unsigned int Room::getLimit() const {
    return _limit;
}

bool Room::isFull() const {
    return _limit > 0 && _members.size() >= _limit;
}

unsigned int Room::getMemberCount() const {
    return _members.size();
}

std::string Room::getMemberList() const {
    std::string list;
    for (size_t i = 0; i < _members.size(); ++i) {
        if (isOperator(_members[i])) {
            list += "@";
        }
        list += _members[i]->getNickname();
        if (i + 1 < _members.size()) {
            list += " ";
        }
    }
    return list;
}

bool Room::isEmpty() const {
    return _members.empty();
}
