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
        std::string invChannel;
        std::string curr_channel;
        bool in_channel;
        bool loggedIn;
        bool op;
        clock_t joinedIn;
    public:
        Client(int sockfd, const std::string& ip);
        ~Client();
        void clearBuffer();

        /*      Setters     */
        void    setChannel(std::string& chname);
        void    setChStatus(bool status);
        void    emptyChannel();
        void    setOpStatus(bool status);
        void    setJoinTime(clock_t time);
        void setUsername(const std::string& name);
        void setPassword(const std::string& pass);
        void setNickname(const std::string& nick);
        void setLog(bool log);
        void setBuffer(const std::string& buff);
        void setinvChannel(std::string chname);

        /*      Getters     */

        
        int getFd();
        std::string getBuffer();
        bool getLog();
        std::string getIpAddress();
        std::string getUsername();
        std::string getPassword();
        std::string getNickname();
        std::string getChannel();
        std::string getinvChannel();
        bool getChStatus();
        bool getOpStatus();
        clock_t getJoinTime();
};

#endif