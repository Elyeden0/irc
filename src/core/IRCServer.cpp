#include "IRCServer.hpp"
#include "Commands.hpp"
#include "Replies.hpp"
#include "Message.hpp"

bool IRCServer::_running = false;

IRCServer::IRCServer(int port, const std::string &password)
	: _password(password), _server_socket(-1), _port(port) {
}

IRCServer::~IRCServer() {
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
	addClient(_server_socket);
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
	_clients.addUser(socket_clientA);
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
	}
}

int	IRCServer::make_poll()
{
	char buffer[1024];
	int res = poll(_fds.data(), _fds.size(), -1);

    if (res < 0)
    {
        error_text("poll doesn't work");
        return error;
    }
    size_t i = 0;
    while (i < _fds.size())
	{
		if (_fds[i].revents == 0) {
			i++;
			continue ;
		}

		if (i == 0) {
			std::cout << "Added a new client!" << std::endl;
			new_client();
			i++;
			continue ;
		}

		if (!(_fds[i].revents & POLLIN)) {
			i++;
			continue;
		}

		int val;
		val = recv(_fds[i].fd, buffer, 1024, 0);
		if (val == 0)
		{
			std::cout << "Removed a client!" << std::endl;
			_clients.removeUser(_fds[i].fd);
			removeClient(_fds[i].fd);
		}
		else if (val < 0)
		{
			i++;
		}
		else {
			buffer[val] = '\0';
			std::cout << "Received: " << buffer << std::endl;
			
			User *user = _clients.getUser(_fds[i].fd);
			if (user) {
				user->getBuffer().append(buffer);
				
				std::string &userBuffer = user->getBuffer();
				size_t pos;
				while ((pos = userBuffer.find(CRLF)) != std::string::npos) {
					std::string line = userBuffer.substr(0, pos);
					userBuffer.erase(0, pos + 2);
					
					try {
						Message msg = parseMessage(line);
						CommandHandler handler = getCommandHandler(msg.command);
						
						if (handler) {
							handler(*this, user, msg);
						}
						
						if (user->hasQuit()) {
							_channels.removeUserFromAll(user);
							_clients.removeUser(_fds[i].fd);
							removeClient(_fds[i].fd);
							break;
						}
					} catch (std::exception &e) {
						std::cerr << "Parse error: " << e.what() << std::endl;
					}
				}
			}
			i++;
		}
	}
	return nothing;
}

void	IRCServer::removeClient(int fd)
{
	for (std::vector<pollfd>::iterator it = _fds.begin() + 1; it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			close(fd);
			_fds.erase(it);
			break;
		}
	}
}

void	IRCServer::NewMessage(void *buffer, int fd)
{
	for (size_t i = 1; i < _fds.size(); ++i)
	{
		if (_fds[i].fd != fd)
		{
			send(_fds[i].fd, buffer, sizeof(buffer), 0);
		}
	}
}

bool IRCServer::start() {
	return (léa(_port) != error);
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
