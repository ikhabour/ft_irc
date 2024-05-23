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

Client::Client(int sockfd, const std::string& ip) : fd(sockfd), ipAddress(ip), curr_channel(""),
in_channel(false)
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

bool    Client::getOpStatus(std::string key)
{
    std::map<std::string, bool>::iterator it;
    it = opMap.find(key);
    if (it != opMap.end())
    {
        if (this->isOnChannel(key) && it->second)
            return true;
    }
    return false;
}

void    Client::setOpStatus(std::string key, bool value)
{
    this->opMap[key] = value;
}

void    Client::setJoinTime(std::string key, clock_t value)
{
    map[key] = value;
}

clock_t Client::getJoinTime(std::string key)
{
    std::map<std::string, clock_t>::iterator it;
    it = map.find(key);
    if (it != map.end())
        return it->second;
    return 0;
}

void    Client::removeFromMap(std::string key)
{
    std::map<std::string, clock_t>::iterator it;
    it = map.find(key);
    if (it != map.end())
    {
        map.erase(it);
        return ;
    }
}

void    Client::addclientChannel(std::string chname)
{
    std::vector<std::string>::iterator it;
    for (it = clientChannels.begin(); it != clientChannels.end(); it++)
    {
        if (*it == chname)
            return ;
    }
    clientChannels.push_back(chname);
}

void    Client::removeclientChannel(std::string chname)
{
    std::vector<std::string>::iterator it;
    for (it = clientChannels.begin(); it != clientChannels.end(); it++)
    {
        if (*it == chname)
        {
            clientChannels.erase(it);
            break;
        }
    }
}

size_t  Client::getChannelsSize()
{
    return this->clientChannels.size();
}

void    Client::setChannel(std::string chname)
{
    this->curr_channel = chname;
}

std::string Client::getChannel()
{
    return this->curr_channel;
}
void    Client::emptyChannel()
{
    this->curr_channel.clear();
}

bool    Client::isOnChannel(std::string chname)
{
    std::vector<std::string>::iterator it;

    for (it = clientChannels.begin(); it != clientChannels.end(); it++)
    {
        if (*it == chname)
            return true;
    }
    return false;
}

std::vector<std::string> Client::returnChannel()
{
    return this->clientChannels;
}


bool    Client::isChatBoxOpen(std::string nickname)
{
    std::vector<std::string>::iterator it;
    for (it = openChatBoxes.begin(); it != openChatBoxes.end(); it++)
    {
        if ((*it) == nickname)
            return true;
    }
    return false;
}

void    Client::addChatBox(std::string nickname)
{
    openChatBoxes.push_back(nickname);
}

void    Client::InvitetoChannel(std::string chname)
{
    this->invChannels.push_back(chname);
}

bool    Client::isInvitedToChannel(std::string chname)
{
    std::vector<std::string>::iterator it;
    for (it = invChannels.begin(); it != invChannels.end(); it++)
    {
        if (*it == chname)
            return true;
    }
    return false;
}