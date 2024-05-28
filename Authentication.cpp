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
		{
			sendMsg(fd, ERR_PASSWDMISMATCH(client->getNickname()));
            std::cerr << "Invalid password\n";
		}
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
	cmd = cmd.substr(4);
	if (only_spaces(cmd))
	{
		sendMsg(fd, "Please enter a proper nickname\n");
		return ;
	}
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
		sendMsg(fd, "Nickname is already in use\n");
		return ;
	}
	else
	{
		client->setNickname(cmd);
		sendMsg(fd, "Nickname set to : " + cmd + "\n");
		std::cout << GRE << "Nickname set successfully to: " << cmd << WHI << std::endl;
	}
	return;
}

void Server::Username(int fd, std::string cmd)
{
	Client *client = getClient(fd);
	cmd = cmd.substr(4);
	if (only_spaces(cmd))
	{
		sendMsg(fd, "Please enter a proper username\n");
		return ;
	}
	std::vector<std::string> cmds = split_cmd(cmd);
	if (!client->getLog())
	{
		std::cerr << "Client not logged in\n";
		sendMsg(fd, ERR_NOTREGISTERED(client->getNickname()));
		return;
	}
	else if (client->getLog() && !client->getNickname().empty())
	{
		client->setUsername(cmds[0]);
		sendMsg(fd, "Username set to : " + cmds[0] + "\n");
		std::cout << GRE << "Username set successfully to: " << cmds[0] << WHI << std::endl;
        sendMsg(fd, RPL_WELCOME(client->getNickname()));
	}
}

bool Server::registration(int fd)
{
	if (!getClient(fd)->getNickname().empty() && !getClient(fd)->getUsername().empty())
		return (true);
	return (false);
}