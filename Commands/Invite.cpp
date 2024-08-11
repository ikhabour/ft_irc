#include "../Server.hpp"

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