#include "User.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

User::User(int fd) 
    : _fd(fd), _authState(AUTH_NONE), _quit(false), _operator(false) {}

User::~User() {
    if (_fd != -1) {
        close(_fd);
    }
}

int User::getFd() const { return _fd; }

AuthState User::getAuthState() const { return _authState; }

void User::setAuthState(AuthState state) { _authState = state; }

const std::string& User::getNickname() const { return _nickname; }
// const std::string& User::getUsername() const { return _username; }
const std::string& User::getRealname() const { return _realname; }
const std::string& User::getHostname() const { return _hostname; }

// void User::setNickname(const std::string &nick) { _nickname = nick; }
// void User::setUsername(const std::string &user) { _username = user; }
// void User::setRealname(const std::string &real) { _realname = real; }
// void User::setHostname(const std::string &host) { _hostname = host; }

std::string User::getPrefix() const {
    return _nickname + "!" + _username + "@" + _hostname;
}

std::string& User::getBuffer() { return _buffer; }

User::ReadResult User::receive() {
    char buf[512];
    ssize_t n = recv(_fd, buf, sizeof(buf), MSG_DONTWAIT);
    
    if (n == -1) return READ_ERROR;
    if (n == 0) return READ_EOF;
    
    _buffer.append(buf, n);
    return READ_OK;
}

void User::send(const std::string &msg) {
    ::send(_fd, msg.c_str(), msg.length(), 0);
}

void User::markQuit() { _quit = true; }

bool User::hasQuit() const { return _quit; }

bool User::isOperator() const { return _operator; }

void User::setOperator(bool op) { _operator = op; }
