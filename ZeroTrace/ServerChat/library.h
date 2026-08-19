#ifndef LIBRARY_H
#define LIBRARY_H

#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <vector>

struct s_message
{
    std::string content;
    std::string recv;
};
enum class ClientState
{
    USERNAME,
    RECEIVER,
    MESSAGE
};
struct client
{
    ClientState state;
    int fd;
    std::string username;
    std::string receiver;
    bool known;
};

#endif