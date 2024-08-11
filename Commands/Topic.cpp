#include "../Server.hpp"


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