#include "../Server.hpp"

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