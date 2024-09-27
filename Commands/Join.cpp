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
    if (!Channel_exists(chname))
    {
        Channel *tmp = makeChannel(client, chname, pw);
        std::cout<<"Channel created"<<std::endl;
        tmp->add_client(client);
        tmp->setOperator(client);
        client->addclientChannel(chname);
        client->setChStatus(true);
        client->setJoinTime(chname, clock());
        sendMsg(fd, RPL_JOIN(client->getNickname(), chname));
        std::string users = tmp->getClients();
        tmp->sendUserList(users);
        sendMsg(fd, RPL_TOPIC(client->getNickname(), chname, tmp->getTopic()));
        sendMsg(fd, RPL_CHANNELMODEIS(client->getNickname(), chname, ""));
        std::cout<< GRE <<"USER : "<<client->getNickname()<<" created " << chname<<std::endl<< WHI;
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
        else if (tmp->chLimited())
        {
            std::cout<<"yes its limited\n";
            std::cout<<"vec size : "<<tmp->getVecSize()<<" limit : "<<tmp->getLimit()<<std::endl;
            if (tmp->getVecSize() + 1 > tmp->getLimit())
            {
                sendMsg(fd, "Channel is full\n");
                return ;
            }
        }
        else if (tmp->getInviteStatus() && !client->isInvitedToChannel(chname))
        {
            sendMsg(fd, "You are not invited to this channel\n");
            return ;
        }
        tmp->add_client(client);
        if (!client->getChannelsSize())
            client->setChStatus(true);
        client->addclientChannel(chname);
        client->setJoinTime(chname, clock());
        tmp->BroadcastResponse(true, fd, RPL_JOIN(client->getNickname(), chname));
        std::string users = tmp->getClients();
        tmp->sendUserList(users);
        tmp->updateTopic(tmp->getTopic());
        std::string modes = tmp->getChannelModes();
        if (modes == "+")
            modes.clear();
        sendMsg(fd, RPL_CHANNELMODEIS(client->getNickname(), chname, modes));
        std::cout<< GRE <<"USER : "<<client->getNickname()<<" joined " << chname<<std::endl<< WHI;
        return ;
    }
}