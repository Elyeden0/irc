#include "IRCServer.hpp"
#include "Commands.hpp"
#include "Replies.hpp"
#include "Message.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
std::string buffer[1024];

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

void	IRCServer::error_text(std::string msg)
{
	std::cerr << msg << " , ( " << strerror(errno) << " ) " << std::endl;
}

int    IRCServer::create_network(int port)
{
	struct sockaddr_in addr;

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket < 0)
	{
		error_text("Creating the socket is impossible");
		return -1;
	}
	fcntl(_server_socket, F_SETFL, O_NONBLOCK);
	int opt = 1;
	setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	int res = bind(_server_socket, (struct sockaddr *)&addr, sizeof(addr));
	if (res == -1)
	{
		error_text("bind() failed");
		return -1;
	}
	listen(_server_socket, SOMAXCONN);
	return 1;
}

int    IRCServer::new_client()
{
	int socket_clientA = accept(_server_socket, NULL, NULL);
	if (socket_clientA < 0)
	{
		error_text("Accept doesn't work");
		return (-1);
	}
	fcntl(socket_clientA, F_SETFL, O_NONBLOCK);
	addClient(socket_clientA);
	return (socket_clientA);
}

void IRCServer::addClient(int fd)
{
    struct pollfd client_poll;
	client_poll.fd = fd;
	client_poll.events = POLLIN;
	client_poll.revents = 0;
	_fds.push_back(client_poll);
}
int    IRCServer::léa(int port)
{
	std::cout << "IRC server started" << std::endl;

	if (create_network(port) == -1)
		return -1;
	while (true)
	{
		int events = make_poll();
		if (events == error)
			return (-1);
		else if (events == client)
			new_client();
	}
}

int	IRCServer::make_poll()
{
	int res = poll(_fds.data(), _fds.size(), -1);
    if (res < 0)
    {
        error_text("poll doesn't work");
        return error;
    }
    int i = 0;
    while (i < _fds.size() && _fds[i].revents == 0)
        i++;
    if (i == 0)
        return client;
    else if (i >= _fds.size())
        return nothing;
    else
    {
        if (recv(_fds[i].fd, buffer, sizeof(buffer), 0) == 0)
		{
			removeClient(_fds[i].fd);
			return (deconexion);
		}
		else
			NewMessage(buffer, _fds[i].fd);
    }
}

void	IRCServer::removeClient(int fd)
{
	for (int i = 1; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == fd)
		{
			close(fd);
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
}

void	IRCServer::NewMessage(void * buffer, int fd)
{
	for (int i = 1; i < _fds.size(); ++i)
	{
		if (_fds[i].fd != fd)
		{
			send(_fds[i].fd, buffer, sizeof(buffer), 0);
		}
	}
}

bool IRCServer::start() {
	// _running = true;
	// std::cout << "IRC server started" << std::endl;
	
	// while (_running) {
	//     int result = _network.pollEvents();
		
	//     if (result == -1) {
	//         if (errno == EINTR) continue;
	//         std::cerr << "Poll error: " << std::strerror(errno) << std::endl;
	//         return false;
	//     }
		
	//     if (_network.hasNewConnection()) {
	//         int clientFd = _network.acceptConnection();
		//     if (clientFd != -1) {
		//         _clients.addUser(clientFd);
		//         User *newUser = _clients.getUser(clientFd);
		//         if (newUser) {
		//             std::string clientAddr = _network.getClientAddress(clientFd);
		//             newUser->setHostname(clientAddr);
		//         }
		//         _network.addClient(clientFd);
		//     }
		// }
		
		// std::vector<int> readyClients = _network.getReadyClients();
		// for (size_t i = 0; i < readyClients.size(); ++i) {
		//     User *user = _clients.getUser(readyClients[i]);
		//     if (!user) continue;
			
			// User::ReadResult res = user->receive();
			// if (res == User::READ_ERROR || res == User::READ_EOF) {
			//     std::cout << "Client disconnected: " << readyClients[i] << std::endl;
			//     _channels.cv
			//     _network.removeClient(readyClients[i]);
			//     _clients.removeUser(readyClients[i]);
			//     continue;
			// }
			
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
