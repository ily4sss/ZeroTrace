#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include<cstring>
int main()
{
    sockaddr_in sock_client;
    inet_pton(AF_INET, "127.0.0.1", &sock_client.sin_addr);;
    sock_client.sin_family = AF_INET;
    sock_client.sin_port = htons(5555);
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        std::cerr<<"Invalid File Descriptor\n";
        return -1;
    }
    std::string buffer;
    std::string input;
    if (connect(client_fd,(struct sockaddr *)&sock_client,
    sizeof(sock_client)) == -1)
    {
        std::cerr<<("Connection failed...");
        close (client_fd);
        return 1;
    }
    do 
    {
        buffer.resize(1024);
        int bytes = recv(client_fd, buffer.data(), buffer.size(), 0);
        if (bytes <=  0)
        {
            std::cerr<<"Server disconneted\n";
            break;
        }
        buffer.resize(bytes);
        std::cout<<buffer;
        if (!std::getline(std::cin, input))
            break;
        send(client_fd, input.data(), input.size(), 0);
    } while (1);
    close(client_fd);
    return 0;
}