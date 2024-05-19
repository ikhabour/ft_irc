/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-bad <oait-bad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 13:45:25 by oait-bad          #+#    #+#             */
/*   Updated: 2024/05/14 15:39:04 by oait-bad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::clientAuth(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    cmd  = cmd.substr(4);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos < cmd.size())
        cmd = cmd.substr(pos);
    if (pos == std::string::npos)
    {
        std::cerr << "Invalid command received\n";
        return;
    }
    else if (!client->getLog())
    {
        std::string pass = cmd;
        if (pass == password)
        {
            client->setLog(true);
            std::cout << GRE << "Client logged in successfully\n" << WHI;
        }
        else
            std::cerr << "Invalid password\n";
    }
}

bool Server::is_validNickname(std::string& nickname)
{
		
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
		return (false);
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '_')
			return (false);
	}
	return (true);
}

bool Server::nickNameInUse(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i]->getNickname() == nickname)
			return (true);
	}
	return (false);
}

void Server::Nickname(int fd, std::string cmd)
{
	Client *client = getClient(fd);
	cmd = cmd.substr(5);
	size_t pos = cmd.find_first_not_of(" \t\v");
	if (pos < cmd.size())
		cmd = cmd.substr(pos);
	if (pos == std::string::npos)
	{
		std::cerr << "Invalid command received\n";
		return ;
	}
	else if (!client->getLog())
	{
		std::cerr << "Client not logged in\n";
		return ;
	}
	else if (!is_validNickname(cmd))
	{
		sendMsg(fd, "Invalid nickname\n");
		return ;
	}
	else if (nickNameInUse(cmd))
	{
		sendMsg(fd, "Nickname already in use, try another one\n");
		return ;
	}
	else
	{
		client->setNickname(cmd);
		std::cout << GRE << "Nickname set successfully to: " << cmd << WHI << std::endl;
	}
	return;
}

void Server::Username(int fd, std::string cmd)
{
	Client *client = getClient(fd);
	cmd = cmd.substr(5);
	size_t pos = cmd.find_first_not_of(" \t\v");
	if (pos < cmd.size())
		cmd = cmd.substr(pos);
	if (pos == std::string::npos)
	{
		std::cerr << "Invalid command received\n";
		return;
	}
	else if (!client->getLog())
	{
		std::cerr << "Client not logged in\n";
		return;
	}
	else if (cmd[0] == ':')
	{
		client->setUsername(cmd);
		std::cout << GRE << "Username set successfully to: " << cmd << WHI << std::endl;
	}
	else
	{
		sendMsg(fd, "Invalid user, the username should start with ':'\n");
	}
}

bool Server::registration(int fd)
{
	if (!getClient(fd)->getNickname().empty() && !getClient(fd)->getUsername().empty())
		return (true);
	return (false);
}