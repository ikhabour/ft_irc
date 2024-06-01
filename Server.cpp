/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-bad <oait-bad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 18:40:40 by oait-bad          #+#    #+#             */
/*   Updated: 2024/05/14 15:40:34 by oait-bad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::sig = false;

Server::Server(int port, std::string pass) : port(port), password(pass)
{
    if (port < 1024 || port > 65535)
    {
        std::cerr << "Invalid port\n";
        _exit(1);
    }
}

Server::~Server()
{
}

void Server::signalHandler(int signum) 
{
    (void)signum;
    std::cout << RED << "\nSignal received. Shutting down the server.\n" << WHI;
    sig = true;
}

void Server::SocketMaker() 
{
    sockaddr_in Addr;
    srvSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvSocketFd == -1)
        throw std::runtime_error("Error creating socket");
    int en = 1;
    if (setsockopt(srvSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
        throw std::runtime_error("Error setting socket option SO_REUSEADDR");
    if (fcntl(srvSocketFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error setting socket to non-blocking mode");
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = inet_addr("localhost");
    Addr.sin_port = htons(port);
    if (bind(srvSocketFd, (struct sockaddr *)&Addr, sizeof(Addr)) == -1)
        throw std::runtime_error("Error binding socket");
    if (listen(srvSocketFd, 5) == -1)
        throw std::runtime_error("Error listening on socket");
    pollfd Fd;
    Fd.fd = srvSocketFd;
    Fd.events = POLLIN;
    Fd.revents = 0;
    fds.push_back(Fd);
}

std::vector<std::string> Server::split_cmd(std::string& cmd)
{
	std::vector<std::string> vec;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return (vec);
}

void sendMsg(int fd, std::string msg)
{
    send(fd, msg.c_str(), msg.size(), 0);
}

void Server::parse_cmd(int fd, std::string cmd)
{
    static int flag = 0;
    if (cmd.empty())
        return;
    std::vector<std::string> vec = split_cmd(cmd);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos != std::string::npos)
        cmd = cmd.substr(pos);
    if (vec.size() && (vec[0] == "PASS" || vec[0] == "pass"))
        clientAuth(fd, cmd);
    else if (vec.size() && (vec[0] == "NICK" || vec[0] == "nick"))
        Nickname(fd, cmd);
    else if (vec.size() && (vec[0] == "USER" || vec[0] == "user"))
        Username(fd, cmd);
    else if (vec.size() && (vec[0].compare(0, 6, "CAP LS")) && !flag)
        flag = 1;
    else if (registration(fd) == true)
    {
        if (vec.size() && vec[0] == "JOIN")
            Join(fd, cmd);
        if (vec.size() && vec[0] == "PART")
            Leave(fd, cmd);
        if (vec.size() && (vec[0] == "PRIVMSG" || vec[0] == "privmsg"))
            privmsg(fd, cmd);
        if (vec.size() && (vec[0] == "LIST"))
            list(fd, cmd);
        if (vec.size() && (vec[0] == "TOPIC" || vec[0] == "topic"))
            topic(fd, cmd);
        if (vec.size() && (vec[0] == "KICK" || vec[0] == "kick"))
            Kick(fd, cmd);
        if (vec.size() && (vec[0] == "INVITE" || vec[0] == "invite"))
            Invite(fd, cmd);
        if (vec.size() && (vec[0] == "MODE" || vec[0] == "mode"))
            Mode(fd, cmd);
    }
    else
    {
        if (!registration(fd))
            sendMsg(fd, "Please make sure you've picked a nickname and a username\n");
        else
            sendMsg(fd, "Invalid command\n");
    }
}

Client *Server::getClient(int fd)
{
	for (size_t i = 0; i < this->clients.size(); i++)
    {
		if (this->clients[i]->getFd() == fd)
			return this->clients[i];
	}
	return NULL;
}

std::vector<std::string> Server::split_recivedBuffer(std::string str)
{
	std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string line;
	while(std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
	return (vec);
}

void Server::addNewClient() 
{
    sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    if (sig)
        return ;
    int clientFd = accept(srvSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
    if (clientFd == -1)
    {
        std::cerr << "Error accepting client\n";
        return;
    }
    pollfd clientPollFd;
    clientPollFd.fd = clientFd;
    clientPollFd.events = POLLIN;
    clientPollFd.revents = 0;
    fds.push_back(clientPollFd);
    Client* client = new Client(clientFd, inet_ntoa(clientAddr.sin_addr));
    clients.push_back(client);
    clientsFds.push_back(client->getFd());
    std::cout << GRE << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << WHI << std::endl;
}

void Server::receiveData(int fd) 
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    std::vector<std::string> cmds;
    Client *client = getClient(fd);
    ssize_t bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0)
    {
        std::cout << RED << "Client disconnected: " << fd << WHI << std::endl;
        std::vector<std::string> client_channels = client->returnChannel();
        if (client_channels.size())
        {
            std::string msg = "Leaving";
            for (size_t i = 0; i < client_channels.size(); i++)
            {
                std::string chname = client_channels[i];
                Channel *tmp = Channel_exists(chname);
                if (tmp)
                {
                    if (client->getOpStatus(chname))
                        tmp->removeOperator(client->getNickname());
                    client->removeclientChannel(chname);
                    client->removeFromMap(chname);
                    tmp->assignNextOp(client);
                    tmp->remove_client(client);
                    if (client->getChannelsSize() == 0)
                    {
                        client->setChStatus(false);
                        client->emptyChannel();
                    }
                    std::string users = tmp->getClients();
                    ch_broadcast(client->getNickname(), fd, chname, "has left your channel (Server Disconnect)");
                    tmp->sendUserList(users);
                    if (tmp->getVecSize() == 0)
                        deleteChannel(chname);
                }
                else
                    continue ;
            }
        }
        // sendMsg(fd, RPL_QUIT(client->getNickname(), "Left"));
        ClearClients(fd, 0);
        close(fd);
    } 
    else 
    {
        std::cout<<"data : "<<buffer<<std::endl;
        client->setBuffer(buffer);
        if (client->getBuffer().find_first_of("\n") == std::string::npos)
            return;
        cmds = split_recivedBuffer(client->getBuffer());
        for (size_t i = 0; i < cmds.size(); i++)
            parse_cmd(fd, cmds[i]);
        if (getClient(fd))
            getClient(fd)->clearBuffer();
    }
}

void Server::ClearClients(int fd, int flag)
{
    if (flag)
    {
        //TODO : clear everything;
        std::vector<Client*>::iterator it = this->clients.begin();
        while (it != clients.end())
        {
            delete *it;
            it++;
        }
        this->clients.clear();
        return;
    }
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->getFd() == fd)
        {
            delete clients[i];
            clients.erase(clients.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }
}

void Server::closeConnections() 
{
    std::vector<int>::iterator it;
    for (it = clientsFds.begin(); it != clientsFds.end(); it++)
    {
       if (*it && *it != srvSocketFd)
        close(*it);
    }
    if (this->srvSocketFd != -1)
    {
        close(srvSocketFd);
    }
}

void    Server::ClearChannels()
{
    std::vector<Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); it++)
    {
        Channel* tmp = *it;
        if (tmp)
            delete tmp;
    }
    _channels.clear();
}

void Server::initServer() 
{
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    SocketMaker();
    std::cout << YEL << "Server initialized and listening on port " << port << std::endl;
}

void Server::startServer() 
{
    std::cout << GRE << "------- Welcome to the IRC Server -------\n" << WHI;
    std::cout << SBLU << "* To connect to the server, use the command: nc <server_ip> <port>\n";
    std::cout << SBLU << "* To disconnect from the server, use the command: Ctrl + C\n" << WHI;
    initServer();
    std::cout << YEL << "Waiting for clients..." << WHI << std::endl;
    while (!sig) 
    {
        if (poll(fds.data(), fds.size(), -1) == -1 && !sig) 
        {
            std::cerr << "Error in poll\n";
            break;
        }
        for (size_t i = 0; i < fds.size(); ++i) 
        {
            if (fds[i].revents & POLLIN) 
            {
                if (fds[i].fd == srvSocketFd)
                    addNewClient();
                else
                    receiveData(fds[i].fd);
            }
        }
    }
    closeConnections();
    ClearClients(srvSocketFd, 1);
    ClearChannels();
    std::cout << GRE << "The server has been shut down successfully.\n Good Bye :)\n" << WHI;
}