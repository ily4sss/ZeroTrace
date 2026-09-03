#include "server.hpp"

Server::Server(int port)
{
    this->port = port;
    this->server_fd = -1;

    if (!db.open("zerotrace.db"))
    {
        std::cout << "Error opening \n";
    }
    if (!db.CreateUserTable())
    {
        std::cout << "Error Creating table \n";
    }
    if (!db.CreateMessagesTable())
    {
        std::cout << "Error Creating message table \n";
    }
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
                            size_t pos;
                            for (size_t j = 0; j < clients.size(); j++)
                            {
                                if (clients[j].fd != client_fd)
                                    continue;
                                clients[j].buffer += input;
                                while (clients[j].buffer.find('\n') != std::string::npos)
                                {
                                    pos = clients[j].buffer.find('\n');
                                    std::string line = clients[j].buffer.substr(0, pos);
                                    clients[j].buffer.erase(0 , pos + 1);
                                    if (clients[j].fd != client_fd)
                                    continue;


                                    if (clients[j].state == ClientState::USERNAME)
                                    {
                                        clients[j].username = line;
                                        bool found = db.CheckUserExist(line);
                                        if (found)
                                        {
                                            clients[j].known = true;
                                            std::cout<<"------------------------------------\n";
                                            std::cout<<"Welcome back "<<line<<'\n';
                                            std::cout<<"------------------------------------\n";
                                        }
                                        else
                                        {
                                            clients[j].known = false;
                                            if (!db.AddUser(line))
                                            {
                                                std::cout<< "Databse failed to AddUser ...\n";
                                                break;
                                            }
                                            std::cout<<"------------------------------------\n";
                                            std::cout << "New user: " << line << '\n';
                                            std::cout<<"------------------------------------\n";

                                        }
                                        clients[j].state = ClientState::RECEIVER;

                                        std::string prompt = "User to chat with: ";
                                        Send(client_fd, prompt);
                                    }


                                    else if (clients[j].state == ClientState::RECEIVER)
                                    {
                                        bool user_exist = db.CheckUserExist(line);
                                        bool user_online = false;


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
                                            if (clients[k].username == line)
                                            {
                                                user_online = true;
                                                break;
                                            }
                                        }

                                        clients[j].receiver = line;
                                        clients[j].state = ClientState::CHAT;

                                        std::string state;

                                        if (user_online)
                                            state = "Online";
                                        else
                                            state = "Offline";

                                        std::string head ="\nChat with " + line +" [" + state + "]"
                                            " (/exit to leave the session)\n";

                                        Send(client_fd, head);

                                        std::vector<s_message> messages =db.GetHistory(clients[j].username,clients[j].receiver);

                                        for (size_t h = 0; h < messages.size(); h++)
                                        {
                                            std::string output =
                                                "[" + messages[h].sender + "]: "
                                                + messages[h].content + "\n";

                                            Send(client_fd, output);
                                        }
                                    }


                                    else if (clients[j].state == ClientState::CHAT)
                                    {
                                        if (line == "/exit")
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
                                        message.content = line;

                                        if (!db.SaveMessage(message.sender,message.recv,message.content))
                                        {
                                            std::cout << "Failed to save message\n";
                                        }

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

                                        std::cout<< message.sender<< " -> "
                                            << message.recv<< ": "<< message.content<< '\n';

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