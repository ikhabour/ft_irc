#include "../Server.hpp"

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
        sendMsg(fd, RPL_CHANNELMODEIS(client->getNickname(), chname, ""));
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
        tmp->sendUserList(users);
        tmp->updateTopic(tmp->getTopic());
        std::string modes = tmp->getChannelModes();
        if (modes == "+")
            modes.clear();
        sendMsg(fd, RPL_CHANNELMODEIS(client->getNickname(), chname, modes));
        return ;
    }
}