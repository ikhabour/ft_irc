#include "../Server.hpp"


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
    if (!ValidChannelName(chname))
    {
        sendMsg(fd, "Invalid channel name\n");
        return ;
    }
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (!chname.empty() && mode.empty())
        {
            std::string modes = tmp->getChannelModes();
            if (modes == "+")
                modes.clear();
            sendMsg(fd, RPL_CHANNELMODEIS(client->getNickname(), chname, modes));
            return ;
        }
    }
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
    if (tmp)
    {
        // if (mode.empty())
        // {
        //     std::cout<<"Was here\n";
        //     return ;
        // }
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