#ifndef SERVER_HPP
#define SERVER_HPP
#include "library.h"
class Server
{
private:
    int server_fd;
    sockaddr_in socket_addr;
    int port;
    std::vector<pollfd> sockets;
    std::vector<client> clients;
std::vector<std::string> known_users;
public:
    Server(int port);
    ~Server();
    void Create();
    void Bind();
    void Listen();
    void Connection();
};

#endif