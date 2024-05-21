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
std::vector<std::string> split_words(std::string& cmd);
std::string removeExtraSpaces(const std::string& input);









//                      Responses   


/* Error Responses */

#define ERR_NOTREGISTERED(source)                       "451 " + source + " :You have not registered"
#define ERR_ALREADYREGISTERED(source)                   "462 " + source + " :You may not register"
#define ERR_PASSWDMISMATCH(source)                      "464 " + source + " :Password is incorrect"
#define ERR_NONICKNAMEGIVEN(source)                     "431 " + source + " :Nickname not given"
#define ERR_NICKNAMEINUSE(source)                       "433 " + source + " " + source  + " :Nickname is already in use"

#define ERR_UNKNOWNCOMMAND(source, command)             "421 " + source + " " + command + " :Unknown command"
#define ERR_NEEDMOREPARAMS(source, command)             "461 " + source + " " + command + " :Not enough parameters"

#define ERR_TOOMANYCHANNELS(source, channel)            "405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(source, channel)               "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOSUCHCHANNEL(source, channel)              "403 " + source + " " + channel + " :No such channel"
#define ERR_BADCHANNELKEY(source, channel)              "475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_CHANNELISFULL(source, channel)              "471 " + source + " " + channel + " :Cannot join channel (+l)"
#define ERR_CANNOTSENDTOCHAN(source, channel)           "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_CHANOPRIVSNEEDED(source, channel)           "482 " + source + " " + channel + " :You're not channel operator"

#define ERR_NOSUCHNICK(source, nickname)                "401 " + source + " " + nickname + " :No such nick/channel"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) "441 " + source + " " + nickname + " " + channel + " :They aren't on that channel"


/* Numeric Responses */

#define RPL_WELCOME(source)                             "001 " + source + " :Welcome " + source + " to the ft_irc network"
#define RPL_NAMREPLY(source, channel, users)            ":localhost 353 " + source + " = " + channel + " :" + users + "\r\n"
#define RPL_ENDOFNAMES(source, channel)                 ":localhost 366 " + source + " " + channel + " :End of /NAMES list.\r\n"
#define RPL_TOPIC(source, channel, topic)               ":localhost 332 " + source + " " + channel + " :" + topic + "\r\n"


/* Command Responses */

#define RPL_JOIN(source, channel)                       ":" + source + " JOIN :" + channel + "\r\n"
#define RPL_PART(source, channel)                       ":" + source + " PART :" + channel + "\r\n"
#define RPL_PING(source, token)                         ":" + source + " PONG :" + token
#define RPL_PRIVMSG(source, target, message)            ":" + source + " PRIVMSG " + target + " :" + message
#define RPL_NOTICE(source, target, message)             ":" + source + " NOTICE " + target + " :" + message
#define RPL_QUIT(source, message)                       ":" + source + " QUIT :Quit: " + message
#define RPL_KICK(source, channel, target, reason)       ":" + source + " KICK " + channel + " " + target + " :" + reason
#define RPL_MODE(source, channel, modes, args)          ":" + source + " MODE " + channel + " " + modes + " " + args

#endif