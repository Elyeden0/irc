#include "IRCServer.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <sstream>

static void setupSignals() {
    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGPIPE);
    sigprocmask(SIG_BLOCK, &signalSet, NULL);
    
    signal(SIGINT, IRCServer::signalHandler);
    signal(SIGTERM, IRCServer::signalHandler);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    
    int port;
    std::istringstream iss(argv[1]);
    if (!(iss >> port) || port < 1024 || port > 65535) {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }
    
    std::string password(argv[2]);
    if (password.empty()) {
        std::cerr << "Password cannot be empty" << std::endl;
        return 1;
    }
    
    try {
        IRCServer server(port, password);
        setupSignals();
        
        if (!server.start()) {
            return 1;
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
