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