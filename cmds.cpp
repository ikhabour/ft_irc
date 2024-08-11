#include "Server.hpp"

Channel* Server::Channel_exists(std::string chname)
{
    if (_channels.size() == 0)
    {
        std::cout<<"returned NULL"<<std::endl;
        return NULL;
    }
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
        // std::cout<<"chstatus  for "<< tmp->getNickname() <<" : "<<tmp->getChStatus()<<std::endl;
        // std::cout<<"onChannel for "<< tmp->getNickname() <<" : "<<tmp->isOnChannel(chname)<<std::endl;
        if (tmp->getChStatus() && tmp->isOnChannel(chname) && tmp->getFd() != excep)
            sendMsg(tmp->getFd(), RPL_PRIVMSG(user_nick, chname, msg));
    }
}

bool    only_spaces(std::string str)
{
    size_t pos = str.find_first_not_of(" \t\v");
    if (pos != std::string::npos)
        return false;
    return true;
}

std::vector<std::string> split_words(std::string& cmd)
{
    std::vector<std::string> vec;
    if (cmd.empty() || only_spaces(cmd))
        return vec;
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
    else if (!cmd.empty())
        vec.push_back(cmd);
    return vec;
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

void    Channel::assignNextOp(Client* target)
{
    if (getVecSize() == 0 || admins.size() >= 1)
        return ;
    std::string chname = this->getName();
    int lowestTime = std::numeric_limits<int>::max();
    Client* minTimeClient = NULL;
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it)->getJoinTime(chname))
        {
            int clientTime = (*it)->getJoinTime(chname);
            if (clientTime < lowestTime)
            {
                lowestTime = clientTime;
                minTimeClient = *it;
            }
        }
    }
    if (minTimeClient)
        setOperator(target, minTimeClient);
}

std::string Channel::getClients()
{
    std::string users;

    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
        if ((*it)->getOpStatus(this->getName()))
            users += '@';
        users += (*it)->getNickname();
        users += ' ';
    }
    return users;
}

void    Channel::sendUserList(std::string users)
{
    std::vector<Client*>::iterator it;
    std::string msg;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
        msg = RPL_NAMREPLY((*it)->getNickname(), this->getName(), users);
        sendMsg((*it)->getFd(), msg);
        msg = RPL_ENDOFNAMES((*it)->getNickname(), this->getName());
        sendMsg((*it)->getFd(), msg);
    }
}

void    Channel::PrintOperators()
{
    std::vector<Client*>::iterator it;
    for (it = admins.begin(); it != admins.end(); it++)
        std::cout<<"User : " << (*it)->getNickname() << " is an Operator on channel : "<<this->getName()<<std::endl;
}


