#include "server.hpp"


Server::Server(int port)
{
    this->port = port;
    this->server_fd = -1;
}
Server::~Server()
{
    if (server_fd != -1)
        close(server_fd);
}
void Server::Create()
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr<<"Socket not created...\n";
        return;
    }
    std::cout<<"Socket created ["<<server_fd<<"]\n";
}
void Server::Bind()
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
void Server::Listen()
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
void Server::Connection()
{
    while (true)
    {
        int socket_result = poll(sockets.data(), sockets.size(), -1);
        if (socket_result == -1)
        {
            std::cout<<"Poll Failed...\n";
            break;
        }
        for (size_t i = 0; i < sockets.size(); i++ )
        {
            if (sockets[i].revents & POLLIN)
            {
                client data_client;

                if (sockets[i].fd == server_fd)
                {
                    pollfd client;

                    client.fd = accept(server_fd, nullptr, nullptr);
                    client.events = POLLIN;
                    client.revents = 0;

                    if (client.fd == -1)
                    {
                        printf("Connection Failed...");
                        continue;
                    }
                    data_client.state = ClientState::USERNAME;
                    data_client.fd = client.fd;
                    data_client.known = false;
                    data_client.username = "";
                    clients.push_back(data_client);
                    std::cout << "Client accepted: " << client.fd << '\n';
                    std::string input = "Enter your username: ";
                    send(client.fd, input.data(), input.size(), 0);

                    sockets.push_back(client);
                }
                else
                {


                    int client_fd = sockets[i].fd;
                    std::string input;
                    input.resize(64);
                    ssize_t bytes_recv = recv(client_fd, input.data(),
                    input.size(), 0);
                    if(bytes_recv == 0)
                    {
                        printf("Lost connection, Dead socket...");
                        close(client_fd);
                        sockets.erase(sockets.begin() + i);
                        if (i > 0)
                            i--;
                        continue;
                    }
                    if (bytes_recv < 0)
                    {
                        std::cerr<<"Recv failed...\n";
                        continue;
                    }
                    input.resize(bytes_recv);
                    for (size_t j = 0; j < clients.size(); j++)
                    {
                        if (clients[j].fd != client_fd)
                            continue;
                        if (clients[j].state == ClientState::USERNAME)
                        {
                            clients[j].username = input;
                            bool found  = false;
                            for (size_t k = 0; k < known_users.size(); k++)
                            {
                                if (known_users[k] == input)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            if (found)
                            {
                                clients[j].known = true;
                                std::cout<<"------------------------------------\n";
                                std::cout<<"Welcome back "<<input<<'\n';
                                std::cout<<"------------------------------------\n";
                            }
                            else
                            {
                                clients[j].known = false;
                                known_users.push_back(input);
                                std::cout<<"------------------------------------\n";
                                std::cout << "New user: " << input << '\n';
                                std::cout<<"------------------------------------\n";

                            }
                            clients[j].state = ClientState::RECEIVER;

                            std::string prompt = "User to send to: ";
                            send(client_fd, prompt.data(), prompt.size(), 0);
                        }
                        else if (clients[j].state == ClientState::RECEIVER)
                        {
                            clients[j].receiver = input;

                            clients[j].state = ClientState::MESSAGE;

                            std::string prompt = "Write your message: ";
                            send(client_fd, prompt.data(), prompt.size(), 0);
                        }
                        else if (clients[j].state == ClientState::MESSAGE)
                        {
                            std::string message = input;
                            bool receiver_found = false;

                            std::cout<< clients[j].username<< " -> "<< clients[j].receiver
                                << ": "<< message<< '\n';

                            for (size_t k = 0; k < clients.size(); k++)
                            {
                                if (clients[k].username == clients[j].receiver)
                                {
                                    std::string output = "\n[" + clients[j].username + "]: " + message + "\n";

                                    send(clients[k].fd,output.data(),output.size(),0);

                                    receiver_found = true;
                                    break;
                                }
                            }

                            if (!receiver_found)
                            {
                                std::string error = "User is not online\n";

                                send(client_fd,error.data(),error.size(),0);
                            }

                            clients[j].state = ClientState::RECEIVER;

                            std::string prompt = "User to send to: ";
                            send(client_fd,prompt.data(),prompt.size(),0);
                        }

                        break;
                    }
                }
                
            }
        }
    }
    
    close(server_fd);
}