#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>

int main()
{
    sockaddr_in sock_client = {};

    sock_client.sin_family = AF_INET;
    sock_client.sin_port = htons(5555);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &sock_client.sin_addr
    );

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd == -1)
    {
        std::cerr << "Invalid File Descriptor\n";
        return 1;
    }

    if (connect(client_fd,(sockaddr *)&sock_client,sizeof(sock_client)) == -1)
    {
        std::cerr << "Connection failed\n";
        close(client_fd);
        return 1;
    }

    pollfd fds[2];

    fds[0].fd = client_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    while (true)
    {
        int result = poll(fds, 2, -1);

        if (result == -1)
        {
            std::cerr << "Poll failed\n";
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            std::string buffer;
            buffer.resize(1024);

            ssize_t bytes = recv(client_fd,buffer.data(),buffer.size(),0);

            if (bytes <= 0)
            {
                std::cout << "\nServer disconnected\n";
                break;
            }

            buffer.resize(bytes);

            std::cout << buffer;
            std::cout.flush();
        }

        if (fds[1].revents & POLLIN)
        {
            std::string input;

            if (!std::getline(std::cin, input))
                break;

            send(client_fd,input.data(),input.size(),0);
        }
    }

    close(client_fd);

    return 0;
}