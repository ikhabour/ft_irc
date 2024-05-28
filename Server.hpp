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



#include <cstdio>


#include <cstring>
#include <string>
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
        std::vector<int> clientsFds;
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
        void ClearClients(int fd, int flag);
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
        void    ClearChannels();

        /*      Commands        */


        void    Join(int fd, std::string cmd);
        void    Leave(int fd, std::string cmd);
        void    privmsg(int fd, std::string cmd);
        void    list(int fd, std::string cmd);
        void    topic(int fd, std::string cmd);

        /*      Operator Commands       */


        void    Kick(int fd, std::string cmd);
        void    Invite(int fd, std::string cmd);
        void    Mode(int fd, std::string cmd);



        /*      Utils       */
        Channel* Channel_exists(std::string chname);
        Channel*    makeChannel(Client* op, std::string& chname, std::string& pw);
        void    deleteChannel(std::string chname);
        Client* srvFindClient(std::string nickname);

        /*      Broadcast       */
        void    ch_broadcast(std::string user_nick, int execp, std::string chname, std::string msg);
};

void sendMsg(int fd, std::string msg);
std::vector<std::string> split_words(std::string& cmd);
std::string removeExtraSpaces(const std::string& input);
bool    only_spaces(std::string str);









//                      Responses   


/* Error Responses */

#define ERR_NOTREGISTERED(source)                       ":0.0.0.0 451 " + source + " :You have not registered"
#define ERR_ALREADYREGISTERED(source)                   ":0.0.0.0 462 " + source + " :You may not register\r\n"
#define ERR_PASSWDMISMATCH(source)                      ":0.0.0.0 464 " + source + " :Password is incorrect\r\n"
#define ERR_NONICKNAMEGIVEN(source)                     "431 " + source + " :Nickname not given"
#define ERR_NICKNAMEINUSE(source)                       ":0.0.0.0 433 " + source + " " + source  + " :Nickname is already in use\r\n"
#define ERR_USERONCHANNEL(target, channel, msg)         ":0.0.0.0 443 " + target + " " + channel + " : " + target + " " + msg + "\r\n"

#define ERR_UNKNOWNCOMMAND(source, command)             "421 " + source + " " + command + " :Unknown command"
#define ERR_NEEDMOREPARAMS(source, command)             ":0.0.0.0 461 " + source + " " + command + " :Not enough parameters\n"

#define ERR_TOOMANYCHANNELS(source, channel)            "405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(source, channel)               "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOSUCHCHANNEL(source, channel)              ":0.0.0.0 403 " + source + " " + channel + " :No such channel\r\n"
#define ERR_BADCHANNELKEY(source, channel)              "475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_CHANNELISFULL(source, channel)              "471 " + source + " " + channel + " :Cannot join channel (+l)"
#define ERR_CANNOTSENDTOCHAN(source, channel)           "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_CHANOPRIVSNEEDED(source, channel)           ":0.0.0.0 482 " + source + " " + channel + " : You're not a channel operator\r\n"

#define ERR_NOSUCHNICK(source, nickname)                ":0.0.0.0 401 " + source + " " + nickname + " :No such nick/channel\r\n"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) ":0.0.0.0 441 " + source + " " + nickname + " " + channel + " :They aren't on that channel\r\n"


/* Numeric Responses */

#define RPL_WELCOME(source)                             ":0.0.0.0 001 " + source + " :Welcome " + source + " to the ft_irc network\r\n"
#define RPL_NAMREPLY(source, channel, users)            ":0.0.0.0 353 " + source + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(source, channel)                 ":0.0.0.0 366 " + source + " " + channel + " :End of /NAMES list.\r\n"
#define RPL_TOPIC(source, channel, topic)               ":0.0.0.0 332 " + source + " " + channel + " :" + topic + "\r\n"

/* List Channel Responses */

#define RPL_LISTSTART(source)                           ":0.0.0.0 321 " + source + " Channel : Users Name\r\n"
#define RPL_LIST(source, channel, visible, topic)       ":0.0.0.0 322 " + source + " " + channel + " " + visible + " :" + topic + "\r\n"
#define RPL_LISTEND(source)                             ":0.0.0.0 323 " + source + " :End of /List\r\n"


/* Command Responses */

#define RPL_JOIN(source, channel)                       ":" + source + " JOIN :" + channel + "\r\n"
#define RPL_PART(source, channel, msg)                  ":" + source + " PART " + channel + " :" + msg + "\r\n"
#define RPL_PING(source, token)                         ":" + source + " PONG :" + token
#define RPL_PRIVMSG(source, target, message)            ":" + source + " PRIVMSG " + target + " :" + message + "\r\n"
#define RPL_NOTICE(source, target, message)             ":" + source + " NOTICE " + target + " :" + message
#define RPL_INVITE(source, target, channel)             ":0.0.0.0 341 " + source + " " + target + " " + channel + "\r\n"
#define RPL_INVITED(source, target, channel)            ":" + source + " INVITE " + target + " " + channel + "\r\n"
#define BROADCAST_INVITE(source, target, channel)       ":0.0.0.0 345 " + source + " " + channel + " " + target + " :test\r\n"
#define RPL_QUIT(source, message)                       ":" + source + " QUIT :Quit: " + message + "\r\n"
#define RPL_KICK(source, channel, target, reason)       ":" + source + " KICK " + channel + " " + target + " :" + reason + "\r\n"
#define RPL_MODE(source, channel, modes, args)          ":" + source + " MODE " + channel + " " + modes + " " + args + "\r\n"

#endif