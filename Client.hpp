/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-bad <oait-bad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 18:20:13 by oait-bad          #+#    #+#             */
/*   Updated: 2024/05/14 15:41:56 by oait-bad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Server.hpp"

class Client 
{
    private:
        int fd;
        std::string buffer;
        std::string ipAddress;
        std::string username;
        std::string password;
        std::string nickname;
        std::vector<std::string> invChannels;
        std::vector<std::string> clientChannels;
        std::map<std::string, clock_t> map;
        std::map<std::string, bool> opMap;
        bool in_channel;
        bool loggedIn;
    public:
        Client(int sockfd, const std::string& ip);
        ~Client();
        void clearBuffer();

        /*      Setters     */


        void    setChStatus(bool status);
        void    setOpStatus(std::string key, bool value);
        void    setJoinTime(std::string key, clock_t value);
        void setUsername(const std::string& name);
        void setPassword(const std::string& pass);
        void setNickname(const std::string& nick);
        void setLog(bool log);
        void setBuffer(const std::string& buff);
        void addclientChannel(std::string chname);
        void removeclientChannel(std::string chname);
        void removeFromMap(std::string key);
        void InvitetoChannel(std::string chname);

        /*      Getters     */

        
        int getFd();
        std::string getBuffer();
        bool getLog();
        std::string getIpAddress();
        std::string getUsername();
        std::string getPassword();
        std::string getNickname();
        bool getChStatus();
        bool getOpStatus(std::string key);
        clock_t getJoinTime(std::string key);
        size_t  getChannelsSize();
        std::string getChannel();
        std::vector<std::string> returnChannel();
        bool    isOnChannel(std::string chname);
        bool    isInvitedToChannel(std::string chname);

        /*      Others      */


        void    PrintMap();
};

#endif