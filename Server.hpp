/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-bad <oait-bad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 12:50:18 by oait-bad          #+#    #+#             */
/*   Updated: 2024/05/14 15:30:51 by oait-bad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"
#define CYA  "\033[36m"
#define MAG "\033[35m"
#define SBLU "\033[38;5;39m"


#include <cstring>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <csignal>
#include <iterator>
#include <limits>
#include <cstdlib>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"


class Channel;

class Client;

class Server
{
    private:
        int port;
        int srvSocketFd;
        std::string password;
        std::vector<Client*> clients;
        std::vector<struct pollfd> fds;
        std::vector<Channel*> _channels;
        static bool sig;
    public:
        Server(int port, std::string pass);
        ~Server();
        void initServer();
        void SocketMaker();
        void addNewClient();
        void receiveData(int fd);
        static void signalHandler(int signum);
        void closeConnections();
        void ClearClients(int fd);
        void parse_cmd(int fd, std::string cmd);
        Client *getClient(int fd);
        void clientAuth(int fd, std::string cmd);
        std::vector<std::string> split_recivedBuffer(std::string str);
        std::vector<std::string> split_cmd(std::string& cmd);
        bool is_validNickname(std::string& nickname);
        bool nickNameInUse(std::string& nickname);
        void Nickname(int fd, std::string cmd);
        void Username(int fd, std::string cmd);
        bool registration(int fd);
        void startServer();

        /*      Commands        */


        void    Join(int fd, std::string cmd);
        void    Leave(int fd, std::string cmd);
        // void    msg(int fd, std::string cmd);
        void    privmsg(int fd, std::string cmd);
        // void    topic(int fd, std::string cmd);

        /*      Operator Commands       */


        // void    Kick(int fd, std::string cmd);
        // void    Invite(int fd, std::string cmd);


        /*      Utils       */
        Channel* Channel_exists(std::string chname);
        Channel*    makeChannel(Client* op, std::string& chname, std::string& pw);
        void    deleteChannel(std::string chname);
        Client* srvFindClient(std::string nickname);

        /*      Broadcast       */
        void    ch_broadcast(std::string user_nick, int execp, std::string chname, std::string msg);
};

void sendMsg(int fd, std::string msg);
std::string removeExtraSpaces(const std::string& input);


#endif