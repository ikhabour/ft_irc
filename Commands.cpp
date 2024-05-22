#include "Server.hpp"

bool isalphanumeric(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!isalnum(str[i]))
            return false;
    }
    return true;
}

std::string removeExtraSpaces(const std::string& input)
{
    std::string result;
    std::istringstream buffer(input);
    std::ostringstream output;

    // Copy non-empty words separated by spaces into the output stream
    std::copy(std::istream_iterator<std::string>(buffer),
              std::istream_iterator<std::string>(),
              std::ostream_iterator<std::string>(output, " "));

    // Remove trailing space if present
    result = output.str();
    if (!result.empty() && result[result.size() - 1] == ' ') {
        result.erase(result.begin() + (result.size() - 1));
    }

    return result;
}


std::string remove_tabs(std::string str)
{
    size_t pos = str.find("\t");
    while (pos != std::string::npos)
    {
        str.replace(pos, 1, " ");
        pos = str.find("\t", pos + 1);
    }
    return str;
}

std::string remove_first_spaces(std::string str)
{
    size_t pos = str.find_first_not_of(" ");
    if (pos != std::string::npos)
        str.erase(0, pos);
    return str;
}


Channel* Server::Channel_exists(std::string chname)
{
    if (_channels.size() == 0)
        return NULL;
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        Channel *tmp = *it;
        if (tmp->getName() == chname)
            return tmp;
    }
    return NULL;
}

Channel*    Server::makeChannel(Client* op, std::string& chname, std::string& pw)
{
    (void)op; 
    Channel* ch = new Channel(chname, pw);
    _channels.push_back(ch);
    return ch;
}

void    Server::deleteChannel(std::string chname)
{
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        Channel* tmp = *it;
        if (tmp->getName() == chname)
        {
            delete tmp;
            _channels.erase(it);
            break ;
        }
    }
}

void    Server::ch_broadcast(std::string user_nick, int excep, std::string chname, std::string msg)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        Client *tmp = *it;
        if (tmp->getChStatus() && tmp->getChannel() == chname && tmp->getFd() != excep)
            sendMsg(tmp->getFd(), "User : " + user_nick + msg);
    }
}


void    Server::Join(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    if (client->getChStatus())
    {
        sendMsg(fd, "You are already on a channel\n");
        return ;
    }
    std::string ch_name;
    std::string pw = "";
	cmd = cmd.substr(4);
	size_t pos = cmd.find_first_not_of(" \t\v");
	if (pos < cmd.size())
		cmd = cmd.substr(pos);
	if (pos == std::string::npos)
	{
		std::cerr << "Invalid command received\n";
        sendMsg(fd, "Wrong Parameters\nUsage : join <channel> <password (if required)>");
		return;
	}
    cmd = removeExtraSpaces(cmd);
    pos = cmd.find(" ");
    if (pos != std::string::npos)
    {
        ch_name = cmd.substr(0, pos);
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        if (pos != std::string::npos) // there is a password after the channel name;
        {
            cmd = cmd.substr(pos);
            size_t pos = cmd.find(" ");
            if (pos != std::string::npos)
                pw = cmd.substr(0, pos);
            else
                pw = cmd;
        }
    }
    else
        ch_name = cmd;

    if (ch_name.length() < 2 && (ch_name[0] == '#' || ch_name[0] == '&'))
    {
        std::cerr<<"Channel name must not contain only \'#\' or \'&\'"<<std::endl;
        return ;
    }
    if (ch_name[0] != '#' && ch_name[0] != '&')
    {
        std::cerr<<"Channel name must start with \'#\' or \'&\'"<<std::endl;
        return ;
    }

    if (!Channel_exists(ch_name)) // if the channel doesn't exist create one and add the client to it
    {
        Channel* tmp = makeChannel(client, ch_name, pw);
        sendMsg(fd, ":" + client->getNickname() + " JOIN " + ch_name + "\r\n");
        tmp->add_client(client);
        client->setChannel(ch_name);
        client->setChStatus(true);
        tmp->setOperator(client);
        if (!client->getJoinTime()) // first time to join
            client->setJoinTime(clock());
    }
    else
    {
        Channel* tmp = Channel_exists(ch_name);
        if (!tmp)
        {
            sendMsg(fd, "Channel does not exist\n");
            return ;
        }
        if (tmp->getPass() == pw) // if the password matches the channel password the client joins
        {
            tmp->add_client(client);
            client->setChannel(ch_name);
            client->setChStatus(true);
            if (!client->getJoinTime()) // first time to join
                client->setJoinTime(clock());
            sendMsg(fd, ":" + client->getNickname() + " JOIN " + ch_name + "\r\n");
            sendMsg(fd, client->getNickname() + " joined " + ch_name + " successfully\n");
            ch_broadcast(client->getNickname(), client->getFd(), ch_name, " joined your channel\n");
        }
        else
            sendMsg(fd, "Wrong Password\n");
    }
}

void    Server::Leave(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    if (!client->getChStatus())
    {
        sendMsg(client->getFd(), "You are not connected to a channel\n");
        return ;
    }
    std::string ch_name;
    cmd = cmd.substr(5);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos < cmd.size())
        cmd = cmd.substr(pos);
    if (pos == std::string::npos)
    {
        std::cerr<<"Invalid command received\n";
        sendMsg(client->getFd(), "Wrong parameters\nUsage : leave <channel>");
        return ;
    }
    cmd = removeExtraSpaces(cmd);
    pos = cmd.find(" ");

    if (pos != std::string::npos)
    {
        pos = cmd.find_first_not_of(" "); // found a character after the space which means there is an extra parameter than the channel name
        if (pos != std::string::npos)
        {
            sendMsg(client->getFd(), "Wrong parameters\nUsage : leave <channel>\n");
            return ;
        }
    }
    else
    {
        ch_name = cmd;
        Channel* tmp = Channel_exists(ch_name);
        if (tmp)
        {
            if (ch_name != client->getChannel())
            {
                sendMsg(fd, "You are not connected to this channel\n");
                return ;
            }
            if (client->getOpStatus())
                tmp->removeOperator(client->getNickname());
            client->setJoinTime(0);
            tmp->assignNextOp();
            client->emptyChannel();
            client->setChStatus(false);
            // tmp->PrintOperators();
            tmp->remove_client(client);
            sendMsg(client->getFd(), "You have left the channel\n");
            ch_broadcast(client->getNickname(), fd, ch_name, " left your channel\n");
            if (tmp->getVecSize() == 0)
                deleteChannel(ch_name);
        }
        else
        {
            sendMsg(client->getFd(), "This channel does not exist\n");
            return ;
        }
    }
}

void    Channel::assignNextOp()
{

    if (getVecSize() == 0 || admins.size() >= 1)
        return ;
    int lowestTime = std::numeric_limits<int>::max();
    Client* minTimeClient = NULL;
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it)->getJoinTime())
        {
            int clientTime = (*it)->getJoinTime();
            if (clientTime < lowestTime)
            {
                lowestTime = clientTime;
                minTimeClient = *it;
            }
        }
    }
    if (minTimeClient)
        setOperator(minTimeClient);
}

void    Channel::PrintOperators()
{
    std::vector<Client*>::iterator it;
    for (it = admins.begin(); it != admins.end(); it++)
        std::cout<<"User : " << (*it)->getNickname() << " is an Operator"<<std::endl;
}



void    Server::msg(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    if (!client->getChStatus())
    {
        sendMsg(fd, "You are not connected to a channel\n");
        return ;
    }
    cmd = cmd.substr(7);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos < cmd.size())
        cmd = cmd.substr(pos);
    if (pos == std::string::npos)
    {
        std::cerr<<"Invalid command received\n";
        sendMsg(client->getFd(), "Wrong parameters\nUsage : msg <message>\n");
        return ;
    }
    cmd = removeExtraSpaces(cmd);
    sendMsg(fd, ":" + client->getNickname() + " PRIVMSG " + "#channel :" + cmd);
    // Channel* tmp = Channel_exists(client->getChannel());
    // if (tmp)
    //     tmp->chsendMsg(cmd);
}


// void    Server::privmsg(int fd, std::string cmd)
// {
//     (void)fd;
//     std::cout<<"cmd : ("<<cmd + ')'<<std::endl;
// }

void    Server::privmsg(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    cmd = cmd.substr(7);
    size_t pos = cmd.find_first_not_of(" \t\v");
    if (pos < cmd.size())
        cmd = cmd.substr(pos);
    if (pos == std::string::npos)
    {
        std::cerr<<"Invalid command received\n";
        sendMsg(fd, "Wrong parameters\nUsage : privmsg <nickname> <message>\n");
        return ;
    }
    cmd = removeExtraSpaces(cmd);
    std::string nickname;
    std::string msg;
    pos = cmd.find(" ");
    if (pos != std::string::npos)
    {
        nickname = cmd.substr(0, pos);
        cmd = cmd.substr(pos);
        pos = cmd.find_first_not_of(" ");
        if (pos != std::string::npos)
            msg = cmd.substr(pos);
    }
    else
        nickname = cmd;
    if (msg.empty())
    {
        sendMsg(fd, "Wrong parameters\nUsage : privmsg <nickname> <message>\n");
        return ;
    }
    Client* to_msg = srvFindClient(nickname);
    // command = "PRIVMSG " + target + " :" + message + "\r\n";
    // send(sockfd, command.c_str(), command.size(), 0);
    if (to_msg)
    {
        sendMsg(fd, "Message sent TO @" + nickname + '\n');
        sendMsg(to_msg->getFd(),":" + client->getNickname() + " PRIVMSG " + to_msg->getNickname() + " :" + msg + "\r\n");
        return ;
    }
    else
    {
        sendMsg(fd, "This user is not connected to the server\n");
        return ;
    }
}
