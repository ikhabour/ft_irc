#include "Server.hpp"

Client* Channel::find_client(std::string nickname)
{
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
        if ((*it)->getNickname() == nickname)
            return *it;
    }
    return NULL;
}


void    Server::Kick(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    if (!client->getChStatus())
    {
        sendMsg(fd, "You not connected to a channel\n");
        return ;
    }
    if (!client->getOpStatus())
    {
        sendMsg(fd, "You are not an operator\n");
        return ;
    }
	cmd = cmd.substr(4);
	size_t pos = cmd.find_first_not_of(" \t\v");
	if (pos < cmd.size())
		cmd = cmd.substr(pos);
	if (pos == std::string::npos)
	{
		std::cerr << "Invalid command received\n";
        sendMsg(fd, "Wrong parameters\nUsage : kick <Nickname> <Reason (optional)>\n");
		return;
	}
    std::string nick;
    std::string reason;
    cmd = removeExtraSpaces(cmd);
    pos = cmd.find(" ");
    if (pos != std::string::npos)
    {
        nick = cmd.substr(0, pos);
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        if (pos != std::string::npos)
            reason = cmd.substr(pos);
        else
            reason = cmd;
    }
    else
        nick = cmd;

    Channel *tmp = Channel_exists(client->getChannel());
    if (tmp)
    {
        Client* to_kick = tmp->find_client(nick);
        if (to_kick)
        {
            if (to_kick->getOpStatus())
            {
                sendMsg(fd, "You cant kick other operators\n");
                return ;
            }
            to_kick->setJoinTime(0);
            to_kick->emptyChannel();
            to_kick->setChStatus(false);
            tmp->remove_client(to_kick);
            sendMsg(fd, "User : " + nick + " was kicked from your channel\n");
            if (reason.empty())
                reason = "No specific reason";
            sendMsg(to_kick->getFd(), "You have been kicked from the channel for : " + reason + '\n');
            ch_broadcast(nick, fd, tmp->getName(), " was kicked from the channel\n");
            return ;
        }
        else
        {
            sendMsg(fd, "This user is not connected to the channel\n");
            return ;
        }
    }
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


void    Server::Invite(int fd, std::string cmd)
{
    Client* client = getClient(fd);
    if (!client->getOpStatus())
    {
        sendMsg(fd, "You are not an operator\n");
        return ;
    }
    cmd = cmd.substr(6);
	size_t pos = cmd.find_first_not_of(" \t\v");
	if (pos < cmd.size())
		cmd = cmd.substr(pos);
	if (pos == std::string::npos)
	{
		std::cerr << "Invalid command received\n";
        sendMsg(fd, "Wrong parameters\nUsage : invite <nickname> <channel>\n");
		return;
	}
    std::string chname;
    std::string nick;
    cmd = removeExtraSpaces(cmd);
    pos = cmd.find(" ");
    if (pos != std::string::npos)
    {
        nick = cmd.substr(0, pos);
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        cmd = cmd.substr(pos);
        pos = cmd.find(" ");
        if (pos != std::string::npos)
        {
            chname = cmd.substr(0, pos);
            cmd = cmd.substr(pos);
            pos = cmd.find(" ");
            if (pos != std::string::npos)
            {
                if (cmd[pos + 1] && cmd[pos + 1] != ' ')
                {
                    sendMsg(fd, "Wrong parameters\nUsage : invite <nickname> <channel>\n");
                    return ;
                }
            }
        }
        else
            chname = cmd;
    }
    else
        nick = cmd;
    if (nick.empty() || chname.empty())
    {
        sendMsg(fd, "Wrong parameters\nUsage : invite <nickname> <channel>\n");
        return ;
    }
    Channel *tmp = Channel_exists(chname);
    if (tmp)
    {
        Client* invClient = tmp->find_client(nick);
        if (invClient)
        {
            invClient->setinvChannel(chname);
            sendMsg(fd, "You have invited @" + nick + "to " + chname + '\n');
            sendMsg(invClient->getFd(), "You have been invited to " + chname + '\n');
            return ;
        }
        else
        {
            sendMsg(fd, "This user is not connected to the server");
            return ;
        }

    }
    else
    {
        sendMsg(fd, "This channel does not exists\n");
        return ;
    }

}