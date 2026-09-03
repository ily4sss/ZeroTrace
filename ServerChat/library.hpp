#ifndef LIBRARY_H
#define LIBRARY_H

#include <iostream>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>


void Bind_Socket();

struct s_message
{
    std::string content;
    std::string recv;
    std::string sender;
};

enum class ClientState
{
    USERNAME,
    RECEIVER,
    CHAT
};

struct client
{
    ClientState state;
    int fd;
    std::string username;
    std::string receiver;
    std::string buffer;
    bool known;
};

#endif