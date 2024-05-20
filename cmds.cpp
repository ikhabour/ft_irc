#include "Server.hpp"

bool isalphanumeric(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!isalnum(str[i]))
            return false;
    }
    return true;
}

Channel* Server::Channel_exists(std::string chname)
{
    if (_channels.size() == 0)
        return NULL;
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        Channel *tmp = *it;
        if (tmp->getName() == chname)
            return tmp;
    }
    return NULL;
}

Channel*    Server::makeChannel(Client* op, std::string& chname, std::string& pw)
{
    (void)op; 
    Channel* ch = new Channel(chname, pw);
    _channels.push_back(ch);
    return ch;
}

void    Server::deleteChannel(std::string chname)
{
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        Channel* tmp = *it;
        if (tmp->getName() == chname)
        {
            delete tmp;
            _channels.erase(it);
            break ;
        }
    }
}

void    Server::ch_broadcast(std::string user_nick, int excep, std::string chname, std::string msg)
{
    // sendMsg(fd, ":" + client->getNickname() + " PRIVMSG " + chname + " :joined your channel\r\n");
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        Client *tmp = *it;
        if (tmp->getChStatus() && tmp->getChannel() == chname && tmp->getFd() != excep)
            sendMsg(tmp->getFd(), ":" + user_nick + " PRIVMSG " + chname + " :" + msg + "\r\n");
    }
}

void    Server::Join(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    std::string chname;
    std::string pw;
    cmd = cmd.substr(5);
    size_t pos = cmd.find(" ");

    if (pos != std::string::npos)
    {
        chname = cmd.substr(0, pos);
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        if (pos != std::string::npos)
            pw = cmd.substr(pos);
    }
    else
        chname = cmd;
    std::cout<<"cmd : ("<<cmd + ')'<<std::endl;
    std::cout<<"chname : ("<<chname + ')'<<std::endl;
    std::cout<<"pw : ("<<pw + ')'<<std::endl;
    if (chname.empty())
    {
        sendMsg(fd, "Please provide a channel name\n");
        return ;
    }
    if (chname[0] != '#' && chname[0] != '&')
    {
        sendMsg(fd, "Channel name must start with \'#\' or \'&\'\n");
        return ;
    }
    if (!Channel_exists(chname))
    {
        Channel *tmp = makeChannel(client, chname, pw);
        tmp->add_client(client);
        tmp->setOperator(client);
        client->setChStatus(true);
        client->setChannel(chname);
        client->setJoinTime(clock());
        sendMsg(fd, ":" + client->getNickname() + " JOIN " + chname + " :\r\n");
        return ;
    }
    else
    {
        Channel *tmp = Channel_exists(chname);
        tmp->add_client(client);
        client->setChStatus(true);
        client->setChannel(chname);
        client->setJoinTime(clock());
        sendMsg(fd, ":" + client->getNickname() + " JOIN " + chname + " :\r\n");
        ch_broadcast(client->getNickname(), fd, chname, "joined your channel");
        return ;
    }
}

bool    only_spaces(std::string str)
{
    size_t pos = str.find_first_not_of(" \t\v");
    if (pos != std::string::npos)
        return false;
    return true;
}

void    Server::privmsg(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    (void)client;

    std::string chname;
    std::string user;
    std::string msg;

    cmd = cmd.substr(7);
    size_t pos;
    std::cout<<"cmd : ("<<cmd + ')'<<std::endl;
    if (cmd.empty() || only_spaces(cmd))
    {
        sendMsg(fd, "Usage : /privmsg <nickname> <message>\n");
        return ;
    }
    cmd = cmd.substr(1);
    if (cmd[0] == '#' || cmd[0] == '&')
    {
        pos = cmd.find(" ");
        if (pos != std::string::npos)
        {
            chname = cmd.substr(0, pos);
            cmd = cmd.substr(pos);
            pos = cmd.find_first_not_of(" ");
            if (pos != std::string::npos)
            {
                
            }
        }
    }
    std::cout<<"cmd  2: ("<<cmd + ')'<<std::endl;
    // else // priv msg executed by using /privmsg
    // {
    //     pos = cmd
    // }
}