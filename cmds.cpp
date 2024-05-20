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
        if (!(tmp->getPass().empty()) && pw != tmp->getPass())
        {
            sendMsg(fd, "Wrong Password\n");
            return;
        }
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

Client* Server::srvFindClient(std::string nickname)
{
    std::vector<Client*>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->getNickname() == nickname)
            return *it;
    }
    return NULL;
}

std::vector<std::string> split_words(std::string& cmd)
{
    std::vector<std::string> vec;
    size_t pos = cmd.find(" ");
    if (pos != std::string::npos)
    {
        vec.push_back(cmd.substr(0, pos));
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        if (pos != std::string::npos)
        {
            cmd = cmd.substr(pos);
            vec.push_back(cmd);
        }
    }
    else
        vec.push_back(cmd);
    return vec;
}

void    Server::privmsg(int fd, std::string cmd)
{
    Client *client = getClient(fd);

    std::string chname;
    std::string user;
    std::string msg;

    cmd = cmd.substr(7);
    if (cmd.empty() || only_spaces(cmd))
    {
        sendMsg(fd, "Usage : /privmsg <nickname> <message>\n");
        return ;
    }
    cmd = cmd.substr(1);
    std::vector<std::string> cmds = split_words(cmd);
    if (cmds.size() < 2 || (only_spaces(cmds[0]) || only_spaces(cmds[1])))
    {
        sendMsg(fd, "Usage : /privmsg <nickname> <message>\n");
        return ;
    }
    if (cmds[0][0] == '#' || cmds[0][0] == '&')
        chname = cmds[0];
    else
        user = cmds[0];
    msg = cmds[1];
    if (!chname.empty())
    {
        if (msg[0] == ':')
            msg.erase(msg.begin());
        ch_broadcast(client->getNickname(), fd, chname, msg);
    }
    else if (!user.empty())
    {
        if (msg.empty())
        {
            sendMsg(fd, "Usage : /privmsg <nickname> <message>\n");
            return ;
        }
        if (msg[0] == ':')
            msg.erase(msg.begin());
        Client *to_msg = srvFindClient(user);
        if (to_msg)
        {
            sendMsg(to_msg->getFd(), ":" + client->getNickname() + " PRIVMSG " + to_msg->getNickname() + " :" + msg + "\r\n");
            return ;
        }
        else
        {
            sendMsg(fd, "This user is not connected to the server\n");
            return ;
        }
    }
}

void    Channel::assignNextOp()
{

    if (getVecSize() == 0 || admins.size() >= 1)
        return ;
    int lowestTime = std::numeric_limits<int>::max();
    Client* minTimeClient = NULL;
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it)->getJoinTime())
        {
            int clientTime = (*it)->getJoinTime();
            if (clientTime < lowestTime)
            {
                lowestTime = clientTime;
                minTimeClient = *it;
            }
        }
    }
    if (minTimeClient)
        setOperator(minTimeClient);
}

void    Server::Leave(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    (void)client;
    cmd = cmd.substr(5);
    std::vector<std::string> cmds = split_words(cmd);
    std::string chname;
    std::string msg;
    for (size_t i = 0; i < cmds.size(); i++)
        std::cout<<"elm : ("<<cmds[i] + ')'<<std::endl;
    chname = cmds[0];
    msg = cmds[1];
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (client->getOpStatus())
            tmp->removeOperator(client->getNickname());
        client->setChStatus(false);
        client->emptyChannel();
        client->setJoinTime(0);
        tmp->assignNextOp();
        tmp->remove_client(client);
        if (only_spaces(msg))
            msg = "Leaving";
        if (msg[0] == ':')
            msg.erase(msg.begin());
        sendMsg(fd, ":" + client->getNickname() + " PART " + chname + " :" + msg + "\r\n");
        ch_broadcast(client->getNickname(), fd, chname, "left your channel");
        return ;
    }
    else
    {
        sendMsg(fd, "This channel does not exists\n");
        return ;
    }
}