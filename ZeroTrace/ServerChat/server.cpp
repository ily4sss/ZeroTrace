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
    this->Bind_Socket();
}
void Server::Listen()
{
    this->Socket_Listen();
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
                            Send(client.fd , input);

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
                                printf("Lost connection, Dead socket...\n");
                                close(client_fd);
                                sockets.erase(sockets.begin() + i);
                                for (size_t j = 0; j < clients.size(); j++)
                                {
                                    if (clients[j].fd == client_fd)
                                    {
                                        clients.erase(clients.begin() + j);
                                        break;
                                    }
                                }
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

                                    std::string prompt = "User to chat with: ";
                                    Send(client_fd, prompt);
                                }
                                else if (clients[j].state == ClientState::RECEIVER)
                                {
                                    bool user_exist = false;
                                    bool user_online = false;

                                    for (size_t k = 0; k < known_users.size(); k++)
                                    {
                                        if (known_users[k] == input)
                                        {
                                            user_exist = true;
                                            break;
                                        }
                                    }

                                    if (!user_exist)
                                    {
                                        std::string prompt =
                                            "User doesn't exist...\n"
                                            "User to chat with: ";

                                        Send(client_fd, prompt);

                                        break;
                                    }

                                    for (size_t k = 0; k < clients.size(); k++)
                                    {
                                        if (clients[k].username == input)
                                        {
                                            user_online = true;
                                            break;
                                        }
                                    }

                                    clients[j].receiver = input;
                                    clients[j].state = ClientState::CHAT;

                                    std::string state;

                                    if (user_online)
                                        state = "Online";
                                    else
                                        state = "Offline";

                                    std::string head ="\nChat with " + input +" [" + state + "]"
                                        " (/exit to leave the session)\n";

                                    Send(client_fd, head);

                                    for (size_t h = 0; h < history.size(); h++)
                                    {
                                        bool sent_by_me =
                                            history[h].sender == clients[j].username &&
                                            history[h].recv == clients[j].receiver;

                                        bool sent_to_me =
                                            history[h].sender == clients[j].receiver &&
                                            history[h].recv == clients[j].username;

                                        if (sent_by_me || sent_to_me)
                                        {
                                            std::string output =
                                                "[" + history[h].sender + "]: " +
                                                history[h].content + "\n";

                                            Send(client_fd, output);
                                        }
                                    }
                                }
                                else if (clients[j].state == ClientState::CHAT)
                                {
                                    if (input == "/exit")
                                    {
                                        clients[j].receiver = "";
                                        clients[j].state = ClientState::RECEIVER;

                                        std::string prompt = "User to chat with: ";

                                        Send(client_fd, prompt);

                                        break;
                                    }

                                    s_message message;

                                    message.sender = clients[j].username;
                                    message.recv = clients[j].receiver;
                                    message.content = input;

                                    history.push_back(message);

                                    std::cout<< message.sender<< " -> "
                                        << message.recv<< ": "<< message.content<< '\n';

                                    for (size_t k = 0; k < clients.size(); k++)
                                    {
                                        if (clients[k].username == clients[j].receiver
                                            && clients[k].state == ClientState::CHAT
                                            && clients[k].receiver == clients[j].username)
                                        {
                                            std::string output =
                                                "\n[" + clients[j].username + "]: "
                                                + message.content + "\n";

                                            Send(clients[k].fd, output);

                                            break;
                                        }
                                    }
                                }

                                break;
                            }
                        }
                        
                    }
            }
    }

close(server_fd);

}