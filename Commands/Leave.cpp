#include "../Server.hpp"

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
    if (cmds.size() > 1)
        msg = cmds[1];
    std::cout<<"msg "<<msg<<std::endl;
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        if (!client->isOnChannel(chname))
        {
            sendMsg(fd, ERR_NOTONCHANNEL(client->getNickname(), chname));
            return ;
        }
        if (client->getOpStatus(chname))
            tmp->removeOperator(client->getNickname());
        client->removeFromMap(chname);
        tmp->assignNextOp();
        if (only_spaces(msg))
            msg = "Leaving";
        if (msg[0] == ':')
            msg.erase(msg.begin());
        tmp->BroadcastResponse(true, fd, RPL_PART(client->getNickname(), chname, msg));
        std::cout<< RED <<"USER : "<<client->getNickname()<<" left " << chname<<std::endl << WHI;
        tmp->remove_client(client);
        if (tmp->chLimited())
            tmp->setLimit(tmp->getLimit() - 1);
        std::string users = tmp->getClients();
        client->removeclientChannel(chname);
        if (client->getChannelsSize() == 0)
            client->setChStatus(false);
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