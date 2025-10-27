#include "IRCServer.hpp"
#include "Commands.hpp"
#include "Replies.hpp"
#include "Message.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>

bool IRCServer::_running = false;

IRCServer::IRCServer(int port, const std::string &password)
    : _password(password) {
    if (!_network.initSocket(port)) {
        throw std::runtime_error("Failed to initialize network");
    }
}

IRCServer::~IRCServer() {
    stop();
}

bool IRCServer::start() {
    _running = true;
    std::cout << "IRC server started" << std::endl;
    
    while (_running) {
        int result = _network.pollEvents();
        
        if (result == -1) {
            if (errno == EINTR) continue;
            std::cerr << "Poll error: " << std::strerror(errno) << std::endl;
            return false;
        }
        
        if (_network.hasNewConnection()) {
            int clientFd = _network.acceptConnection();
            if (clientFd != -1) {
                _clients.addUser(clientFd);
                User *newUser = _clients.getUser(clientFd);
                if (newUser) {
                    std::string clientAddr = _network.getClientAddress(clientFd);
                    newUser->setHostname(clientAddr);
                }
                _network.addClient(clientFd);
            }
        }
        
        std::vector<int> readyClients = _network.getReadyClients();
        for (size_t i = 0; i < readyClients.size(); ++i) {
            User *user = _clients.getUser(readyClients[i]);
            if (!user) continue;
            
            User::ReadResult res = user->receive();
            if (res == User::READ_ERROR || res == User::READ_EOF) {
                std::cout << "Client disconnected: " << readyClients[i] << std::endl;
                _channels.removeUserFromAll(user);
                _network.removeClient(readyClients[i]);
                _clients.removeUser(readyClients[i]);
                continue;
            }
            
            std::string &buffer = user->getBuffer();
            size_t pos;
            while ((pos = buffer.find(CRLF)) != std::string::npos) {
                std::string line = buffer.substr(0, pos);
                buffer.erase(0, pos + 2);
                
                try {
                    Message msg = parseMessage(line);
                    CommandHandler handler = getCommandHandler(msg.command);
                    
                    if (handler) {
                        handler(*this, user, msg);
                    } else {
                        if (user->getAuthState() == AUTH_DONE) {
                            user->send(ERR_UNKNOWNCOMMAND(user->getNickname(), msg.command));
                        }
                    }
                    
                    if (user->hasQuit()) {
                        _channels.removeUserFromAll(user);
                        _network.removeClient(readyClients[i]);
                        _clients.removeUser(readyClients[i]);
                        break;
                    }
                } catch (std::exception &e) {
                    std::cerr << "Parse error: " << e.what() << std::endl;
                }
            }
        }
    }
    
    return true;
}

void IRCServer::stop() {
    _running = false;
}

const std::string& IRCServer::getPassword() const {
    return _password;
}

ClientManager& IRCServer::getClients() {
    return _clients;
}

ChannelManager& IRCServer::getChannels() {
    return _channels;
}

void IRCServer::signalHandler(int signal) {
    (void)signal;
    _running = false;
    std::cout << "\nShutting down server..." << std::endl;
}
