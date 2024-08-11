#include "../Server.hpp"


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