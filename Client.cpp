/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-bad <oait-bad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 18:49:57 by oait-bad          #+#    #+#             */
/*   Updated: 2024/05/14 15:42:07 by oait-bad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int sockfd, const std::string& ip) : fd(sockfd), ipAddress(ip), invChannel(""),
curr_channel(""), in_channel(false), op(false), joinedIn(0)
{
    loggedIn = false;
}

Client::~Client()
{
}

int Client::getFd()
{
    return fd;
}

void Client::setNickname(const std::string& nick)
{
    nickname = nick;
}

void Client::setUsername(const std::string& name)
{
    username = name;
}

void Client::setPassword(const std::string& pass)
{
    password = pass;
}

void Client::setLog(bool log)
{
    loggedIn = log;
}


bool Client::getLog()
{
    return loggedIn;
}

void Client::setBuffer(const std::string& buff)
{
    buffer = buff;
}

std::string Client::getBuffer()
{
    return buffer;
}

void Client::clearBuffer()
{
    buffer.clear();
}

std::string Client::getIpAddress()
{
    return ipAddress;
}

std::string Client::getUsername()
{
    return username;
}

std::string Client::getPassword()
{
    return password;
}

std::string Client::getNickname()
{
    return nickname;
}


void    Client::setChStatus(bool status)
{
    this->in_channel = status;
}

bool Client::getChStatus()
{
    return this->in_channel;
}

void    Client::setChannel(std::string& chname)
{
    this->curr_channel = chname;
}

std::string Client::getChannel()
{
    return this->curr_channel;
}

void    Client::emptyChannel()
{
    if (!curr_channel.empty())
        this->curr_channel.clear();
}

bool    Client::getOpStatus()
{
    return this->op;
}

void    Client::setOpStatus(bool status)
{
    this->op = status;
}

void    Client::setJoinTime(clock_t time)
{
    this->joinedIn = time;
}

clock_t Client::getJoinTime()
{
    return this->joinedIn;
}

void    Client::setinvChannel(std::string chname)
{
    this->invChannel = chname;
}

std::string Client::getinvChannel()
{
    return this->invChannel;
}

// void Client::authenticate()
// {
//     char buffer[1024] = {0};
//     int valread = read(fd, buffer, 1024);
//     if (valread == -1)
//     {
//         std::cerr << "Error reading from client\n";
//         return;
//     }
//     std::string msg(buffer);
//     if (msg.find("PASS") == 0)
//     {
//         password = msg.substr(5, msg.size() - 5);
//         std::cout << "Password received: " << password << std::endl;
//     }
//     else if (msg.find("USER") == 0)
//     {
//         username = msg.substr(5, msg.size() - 5);
//         std::cout << "Username received: " << username << std::endl;
//     }
//     else if (msg.find("NICK") == 0)
//     {
//         nickname = msg.substr(5, msg.size() - 5);
//         std::cout << "Nickname received: " << nickname << std::endl;
//     }
//     else
//     {
//         std::cerr << "Invalid command received\n";
//     }
// }


