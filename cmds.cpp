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

bool    ValidChannelName(std::string chname)
{
    if (chname.empty())
        return false;
    else if ((chname[0] == '#' || chname[0] == '&') && !isalphanumeric(chname.substr(1)))
        return false;
    else if (chname.length() == 1 && (chname[0] == '&' || chname[0] == '#'))
        return false;
    else if (chname[0] != '#' && chname[0] != '&')
        return false;
    return true;
}

void    Server::Join(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    std::string chname;
    std::string pw;
    cmd = cmd.substr(4);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::vector<std::string> cmds = split_words(cmd);
    if (!cmds.size())
    {
        sendMsg(fd, "Usage : /join <channel> <password if required>\n");
        return ;
    }
    chname = cmds[0];
    if (cmds.size() > 1)
        pw = cmds[1];
    if (!ValidChannelName(chname))
    {
        sendMsg(fd, "Invalid Channel Name\n");
        return ;
    }
    // if (chname.empty())
    // {
    //     sendMsg(fd, "Please provide a channel name\n");
    //     return ;
    // }
    // if (chname[0] != '#' && chname[0] != '&')
    // {
    //     sendMsg(fd, "Channel name must start with \'#\' or \'&\'\n");
    //     return ;
    // }
    if (!Channel_exists(chname))
    {
        Channel *tmp = makeChannel(client, chname, pw);
        std::cout<<"Channel created"<<std::endl;
        tmp->add_client(client);
        tmp->setOperator(client, client);
        client->addclientChannel(chname);
        client->setChStatus(true);
        // client->setChannel(chname);
        client->setJoinTime(chname, clock());

        sendMsg(fd, RPL_JOIN(client->getNickname(), chname));
        std::string users = tmp->getClients();
        tmp->sendUserList(users);
        sendMsg(fd, RPL_TOPIC(client->getNickname(), chname, tmp->getTopic()));
        return ;
    }
    else
    {
        Channel *tmp = Channel_exists(chname);
        if (!(tmp->getPass().empty()) && pw != tmp->getPass())
        {
            sendMsg(fd, ERR_PASSWDMISMATCH(client->getNickname()));
            return;
        }
        else if (tmp->chLimited()) // channel is limited
        {
            if (tmp->getVecSize() >= tmp->getLimit())
            {
                sendMsg(fd, "Channel is full\n");
                return ;
            }
            else
                tmp->setLimit(tmp->getLimit() + 1);
        }
        else if (tmp->getInviteStatus() && !client->isInvitedToChannel(chname)) // Channel is invite Only
        {
            sendMsg(fd, "You are not invited to this channel\n");
            return ;
        }
        tmp->add_client(client);
        if (!client->getChannelsSize())
            client->setChStatus(true);
        client->addclientChannel(chname);
        // client->setChannel(chname);
        client->setJoinTime(chname, clock());
        tmp->BroadcastResponse(true, fd, RPL_JOIN(client->getNickname(), chname));
        std::string users = tmp->getClients();
        std::cout<<"Users 2 : "<<users <<std::endl;
        tmp->sendUserList(users);
        tmp->updateTopic(tmp->getTopic());
        return ;
    }
}

void    Channel::PrintOperators()
{
    std::vector<Client*>::iterator it;
    for (it = admins.begin(); it != admins.end(); it++)
        std::cout<<"User : " << (*it)->getNickname() << " is an Operator on channel : "<<this->getName()<<std::endl;
}

void    Server::Leave(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    cmd = cmd.substr(4);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::vector<std::string> cmds = split_words(cmd);
    std::string chname;
    std::string msg;
    if (!cmds.size())
    {
        sendMsg(fd, "Usage : /part <channel> <message (optional)>\n");
        return ;
    }
    chname = cmds[0];
    if (!ValidChannelName(chname))
    {
        sendMsg(fd, "Invalid channel name\n");
        return ;
    }
    msg = cmds[1];
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (client->getOpStatus(chname))
            tmp->removeOperator(client->getNickname());
        client->removeFromMap(chname);
        tmp->assignNextOp(client);
        if (only_spaces(msg))
            msg = "Leaving";
        if (msg[0] == ':')
            msg.erase(msg.begin());
        tmp->BroadcastResponse(true, fd, RPL_PART(client->getNickname(), chname, msg));
        // tmp->sendLeave(RPL_PART(client->getNickname(), chname, msg));
        tmp->remove_client(client);
        if (tmp->chLimited() && tmp->getLimit())
            tmp->setLimit(tmp->getLimit() - 1);
        std::string users = tmp->getClients();
        client->removeclientChannel(chname);
        if (client->getChannelsSize() == 0)
        {
            client->setChStatus(false);
            client->emptyChannel();
        }
        // tmp->PrintOperators();
        tmp->sendUserList(users);
        if (tmp->getVecSize() == 0)
            deleteChannel(chname);
        return ;
    }
    else
    {
        sendMsg(fd, ERR_NOSUCHCHANNEL(client->getNickname(), chname));
        return ;
    }
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
        if (!ValidChannelName(chname))
        {
            sendMsg(fd, "Invalid channel name\n");
            return ;
        }
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
            sendMsg(to_msg->getFd(), RPL_PRIVMSG(client->getNickname(), to_msg->getNickname(), msg));
            if (!client->isChatBoxOpen(user) && !to_msg->isChatBoxOpen(client->getNickname()))
            {
                sendMsg(fd, RPL_PRIVMSG(to_msg->getNickname(), client->getNickname(), ""));
                client->addChatBox(user);
                to_msg->addChatBox(client->getNickname());
            }
            return ;
        }
        else
        {
            sendMsg(fd, "This user is not connected to the server\n");
            return ;
        }
    }
}


void    Server::list(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    (void)cmd;
    
    sendMsg(fd, RPL_LISTSTART(client->getNickname()));
    for (size_t i = 0; i < _channels.size(); i++)
    {
        size_t visible = _channels[i]->getVecSize();
        std::stringstream ss;
        ss << visible;
        std::string msg = RPL_LIST(client->getNickname(), _channels[i]->getName(), ss.str(), _channels[i]->getTopic());
        sendMsg(fd, msg);
    }
    sendMsg(fd, RPL_LISTEND(client->getNickname()));
}


void    Server::topic(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    cmd = cmd.substr(5);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::vector<std::string> cmds = split_words(cmd);
    if (!cmds.size())
    {
        sendMsg(fd, "Usage : /topic <channel> <topic to set>\n");
        return ;
    }
    std::string chname;
    std::string topic;

    if (!cmds[0].empty())
        chname = cmds[0];
    if (!ValidChannelName(chname))
    {
        sendMsg(fd, "Invalid channel name\n");
        return ;
    }
    if (cmds.size() > 1 && !cmds[1].empty())
        topic = cmds[1];
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (topic.empty())
        {
            sendMsg(fd, RPL_TOPIC(client->getNickname(), chname, tmp->getTopic()));
            return ;
        }
        if (!client->getOpStatus(chname) && tmp->getTopicStatus())
        {
            sendMsg(fd, ERR_CHANOPRIVSNEEDED(client->getNickname(), chname));
            return ;
        }
        if (topic[0] == ':')
            topic.erase(topic.begin());
        if (topic.empty() || only_spaces(topic))
            topic = "No topic is set";
        if (tmp->getTopicStatus() && !client->getOpStatus(chname))
        {
            sendMsg(fd, ERR_CHANOPRIVSNEEDED(client->getNickname(), chname));
            return ;
        }
        tmp->setTopic(topic);
        tmp->updateTopic(topic);
        return ;
    }
    else
    {
        sendMsg(fd, ERR_NOSUCHCHANNEL(client->getNickname(), chname));
        return ;
    }
}

void    Server::Kick(int fd, std::string cmd)
{
    Client *client = getClient(fd);
	cmd = cmd.substr(4);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::string chname;
    std::vector<std::string> cmds = split_words(cmd);
    if (!cmds.size())
    {
        sendMsg(fd, "Usage : /kick <nickname> <reason (optional)>\n");
        return ;
    }

    if (!cmds[0].empty())
        chname = cmds[0];
    if (cmds.size() > 1 && !cmds[1].empty())
        cmds = split_words(cmds[1]);    
    if (cmds[0] == chname)
    {
        sendMsg(fd, "Usage : /kick <nickname> <reason (optional)>\n");
        return ;
    }
    std::string target;
    std::string reason;


    if (!cmds[0].empty())
    {
        target = cmds[0];
        if (target[0] == '#' || target[0] == '&')
        {
            sendMsg(fd, "Nickname must not start with \'#\' or \'&\'\n");
            return ;
        }
    }
    if (!cmds[1].empty())
    {
        reason = cmds[1];
        if (reason == target)
            reason = "No reason";
        else
            reason.erase(reason.begin());
    }
    if (chname != "localhost") // command sent from a channel
    {
        if (!ValidChannelName(chname))
        {
            sendMsg(fd, "Invalid channel name\n");
            return ;
        }
        Channel *tmp = Channel_exists(chname);
        if (tmp)
        {
            if (!client->getOpStatus(chname))
            {
                sendMsg(fd, ERR_CHANOPRIVSNEEDED(client->getNickname(), chname));
                return ;
            }
            Client* tg = srvFindClient(target);
            if (!tg)
            {
                sendMsg(fd, ERR_USERNOTINCHANNEL(client->getNickname(), target, chname));
                return ;
            }
            if (tg->getOpStatus(chname))
                tmp->removeOperator(tg->getNickname());
            tg->removeclientChannel(chname);
            tg->removeFromMap(chname);
            tmp->assignNextOp(client);
            tmp->BroadcastResponse(true, fd, RPL_KICK(client->getNickname(), chname, target, reason));
            // tmp->sendKick(RPL_KICK(client->getNickname(), chname, target, reason));
            tmp->remove_client(tg);
            if (tg->getChannelsSize() == 0)
            {
                tg->setChStatus(false);
                tg->emptyChannel();
            }
            std::string users = tmp->getClients();
            tmp->sendUserList(users);
            sendMsg(tg->getFd(), RPL_PART(tg->getNickname(), chname, "Leaving"));
            if (tmp->getVecSize() == 0)
                deleteChannel(chname);
            return ;
        }
        else
        {
            sendMsg(fd, ERR_NOSUCHCHANNEL(client->getNickname(), chname));
            return ;
        }
    }
    else
    {
        sendMsg(fd, "Please use /kick on a channel window\n");
        return ;
    }
}


void    Server::Invite(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    cmd = cmd.substr(6);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::vector<std::string> cmds = split_words(cmd);
    std::string chname;
    std::string target;
    if (!cmds.size())
    {
        sendMsg(fd, "Usage : /invite <nickname> <channel>\n");
        return ;
    }
    if (!cmds[0].empty())
        target = cmds[0];
    if (!cmds[1].empty())
        chname = cmds[1];
    if (chname != "localhost") // channel invite, not from server tab
    {
        if (!ValidChannelName(chname))
        {
            sendMsg(fd, "Invalid channel name\n");
            return ;
        }
        Channel *tmp = Channel_exists(chname);
        if (tmp)
        {
            if (!client->getOpStatus(chname))
            {
                sendMsg(fd, ERR_CHANOPRIVSNEEDED(client->getNickname(), chname));
                return ;
            }
            Client* usr = srvFindClient(target);
            if (usr)
            {
                if (usr->isOnChannel(chname))
                {
                    sendMsg(fd, ERR_USERONCHANNEL(target, chname, "is already on channel"));
                    return ;
                }
                if (usr->isInvitedToChannel(chname))
                {
                    sendMsg(fd, ERR_USERONCHANNEL(target, chname, "is already invited to channel"));
                    return;
                }
                usr->InvitetoChannel(chname);
                sendMsg(fd, RPL_INVITE(client->getNickname(), usr->getNickname(), chname));
                tmp->BroadcastResponse(false, fd, RPL_INVITED(client->getNickname(), target, chname));
                sendMsg(usr->getFd(), RPL_INVITED(client->getNickname(), target, chname));
                return ;
            }
            else
            {
                sendMsg(fd, ERR_NOSUCHNICK(client->getNickname(), target));
                return ;
            }
        }
        else
        {
            sendMsg(fd, ERR_NOSUCHCHANNEL(client->getNickname(), chname));
            return ;
        }
    }
    else
    {
        sendMsg(fd, "Usage : /invite <nickname> <channel>\n");
        return ;
    }
}

bool    isvalidMode(std::string mode)
{
    if (mode.length() < 2 || mode.length() > 2)
        return false;
    if (mode[0] == '+' || mode[0] == '-')
    {
        if (mode[1] == 'l' || mode[1] == 't' || mode[1] == 'i' || mode[1] == 'k' || mode[1] == 'o')
            return true;
        else
            return false;
    }
    return false;
}

bool    requiresParam(std::string mode)
{
    if (mode == "+o" || mode == "-o" || mode == "+k" || mode == "+l")
        return true;
    return false;
}

bool    isNumeric(std::string param)
{
    for (size_t i = 0; i < param.size(); i++)
    {
        if (!isdigit(param[i]))
            return false;
    }
    return true;
}

void    Server::Mode(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    cmd = cmd.substr(4);
    if (cmd[0] && cmd[0] == ' ')
        cmd.erase(cmd.begin());
    std::string chname;
    std::string mode;
    std::string param;
    std::vector<std::string> cmds = split_cmd(cmd);
    if (cmds.size())
    {
        chname = cmds[0];
        if (cmds.size() > 1)
            mode = cmds[1];
    }
    if (chname.empty())
    {
        sendMsg(fd, "Usage : /mode <channel> <mode> <3rd parameter (if required)>\n");
        return ;
    }
    else if (!chname.empty() && mode.empty())
        return ;
    if (!isvalidMode(mode))
    {
        sendMsg(fd, "Invalid mode, please use one of these modes : i | l | t | o | k\n");
        return ;
    }
    if (requiresParam(mode))
    {
        if (cmds.size() > 2)
            param = cmds[2];
        else
        {
            sendMsg(fd, "Usage : /mode <channel> <mode> <3rd parameter>\n");
            return ;
        }
    }
    if (!ValidChannelName(chname))
    {
        sendMsg(fd, "Invalid channel name\n");
        return ;
    }
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (!client->getOpStatus(chname))
        {
            sendMsg(fd, ERR_CHANOPRIVSNEEDED(client->getNickname(), chname));
            return ;
        }
        if (mode == "+k")
        {
            tmp->setPass(param);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "+k", param));
            return ;
        }
        else if (mode == "-k")
        {
            tmp->clearPass();
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "-k", param));
            return ;
        }
        else if (mode == "+l")
        {
            if (!isNumeric(param))
            {
                sendMsg(fd, "Error : Limit must be a number\n");
                return ;
            }
            else if (std::atoi(param.c_str()) <= 0)
            {
                sendMsg(fd, "Error : Limit must be a positive number\n");
                return ;
            }
            else if ((size_t)std::atoi(param.c_str()) < tmp->getVecSize())
            {
                sendMsg(fd, "Error : Limit must be greater than or equal to the current connected users\n");
                return ;
            }
            tmp->LimitChannel(true);
            tmp->setLimit(std::atoi(param.c_str()));
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "+l", param));
            return ;
        }
        else if (mode == "-l")
        {
            tmp->LimitChannel(false);
            tmp->setLimit(0);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "-l", param));
            return ;
        }
        else if (mode == "+i")
        {
            tmp->setInviteOnly(true);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "+i", param));
        }
        else if (mode == "-i")
        {
            tmp->setInviteOnly(false);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "-i", param));
        }
        else if (mode == "+o")
        {
            Client *target = srvFindClient(param);
            if (target)
            {
                if (!target->isOnChannel(chname))
                {
                    sendMsg(fd, ERR_USERNOTINCHANNEL(client->getNickname(), param, chname));
                    return ;
                }
                else if (target->getOpStatus(chname))
                {
                    sendMsg(fd, "This user is already an operator\n");
                    return ;
                }
                tmp->setOperator(client, target);
                tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "+o", param));
                std::string users = tmp->getClients();
                tmp->sendUserList(users);
            }
            else
            {
                sendMsg(fd, ERR_NOSUCHNICK(client->getNickname(), param));
                return ;
            }
        }
        else if (mode == "-o")
        {
            Client *target = srvFindClient(param);
            if (target)
            {
                if (!target->isOnChannel(chname))
                {
                    sendMsg(fd, ERR_USERNOTINCHANNEL(client->getNickname(), param, chname));
                    return ;
                }
                else if (!target->getOpStatus(chname))
                {
                    sendMsg(fd, "This user is not an operator\n");
                    return ;
                }
                tmp->removeOperator(param);
                tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "-o", param));
                std::string users = tmp->getClients();
                tmp->sendUserList(users);
            }
               else
            {
                sendMsg(fd, ERR_NOSUCHNICK(client->getNickname(), param));
                return ;
            }
        }
        else if (mode == "+t")
        {
            tmp->restrictTopic(true);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "+t", param));
            return ;
        }
        else if (mode == "-t")
        {
            tmp->restrictTopic(false);
            tmp->BroadcastResponse(true, fd, RPL_MODE(client->getNickname(), chname, "-t", param));
            return ;
        }
    }
    else
    {
        sendMsg(fd, ERR_NOSUCHCHANNEL(client->getNickname(), chname));
        return ;
    }
}   