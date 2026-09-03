#ifndef SERVER_HPP
#define SERVER_HPP

#include "library.hpp"
#include "database.hpp"

class Server
{
private:
    DataBase db; 
    int server_fd;
    sockaddr_in socket_addr;
    int port;
    std::vector<pollfd> sockets;
    std::vector<client> clients;


    //--------------Funcs helpers-------
    void Bind_Socket()
    {
        socket_addr.sin_family = AF_INET;
        socket_addr.sin_addr.s_addr = INADDR_ANY;
        socket_addr.sin_port = htons(port);
        if (bind(server_fd, (sockaddr *)&socket_addr, sizeof(sockaddr_in)) == -1)
        {
            std::cerr<<"Bind failed...\n";
            return;
        }
        std::cout<<"[Bind is fine]\n";
    }
    void Socket_Listen()
    {
        if (listen(server_fd, 1) == -1)
        {
            std::cerr<<"Listen failed...\n";
            return;
        }

        pollfd server_poll;
        server_poll.fd = server_fd;
        server_poll.events = POLLIN;
        server_poll.revents = 0;

        sockets.push_back(server_poll);
        std::cout<<"Listenning...\n";
    }


public:
    Server(int port);
    ~Server();
    void Create();
    void Bind();
    void Listen();
    void Connection();

    void Send(int fd, const std::string& message)
    {
        send(fd,message.data(),message.size(),0);
    }

};

#endif