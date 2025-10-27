#ifndef USER_HPP
#define USER_HPP

#include <string>

enum AuthState {
    AUTH_NONE,
    AUTH_PASS,
    AUTH_NICK,
    AUTH_DONE
};

class User {
public:
    User(int fd);
    ~User();
    
    int getFd() const;
    AuthState getAuthState() const;
    void setAuthState(AuthState state);
    
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    
    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setRealname(const std::string &real);
    void setHostname(const std::string &host);
    
    std::string getPrefix() const;
    std::string& getBuffer();
    
    enum ReadResult { READ_ERROR = -1, READ_EOF, READ_OK };
    ReadResult receive();
    void send(const std::string &msg);
    
    void markQuit();
    bool hasQuit() const;
    
    bool isOperator() const;
    void setOperator(bool op);
    
private:
    const int _fd;
    std::string _buffer;
    AuthState _authState;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    bool _quit;
    bool _operator;
};

#endif
