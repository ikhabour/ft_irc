#include "Channel.hpp"


Channel::Channel(std::string& chname, std::string& key) : _name(chname), _password(key), _topic("No topic is set"), _i(false), _t(false), _l(0)
{}

void    Channel::setName(std::string& Name)
{
    this->_name = Name;
}

void    Channel::setLimit(int lim)
{
    this->_l = lim;
}

void    Channel::setTopic(std::string& Topic)
{
    this->_topic = Topic;
}

void    Channel::setInviteOnly(bool status)
{
    this->_i = status;
}

void    Channel::restrictTopic(bool status)
{
    this->_t = status;
}

std::string Channel::getName() const
{
    return this->_name;
}

std::string Channel::getPass() const
{
    return this->_password;
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

int Channel::getLimit() const
{
    return this->_l;
}

bool Channel::getInviteStatus()
{
    return this->_i;
}

bool Channel::getTopicStatus()
{
    return this->_t;
}

void    Channel::setOperator(Client* client)
{
    client->setOpStatus(this->getName(), true);
    // :nickname!username@hostname MODE #channelname +o targetnickname
    std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost" + " MODE " + this->getName() + " +o " + client->getNickname() +"\r\n";
    sendMsg(client->getFd(), msg);
    admins.push_back(client);
}

void    Channel::removeOperator(std::string nickname)
{
    std::vector<Client*>::iterator it;
    for (it = admins.begin(); it != admins.end(); it++)
    {
        if ((*it)->getNickname() == nickname)
        {
            (*it)->setOpStatus(this->getName(), false);
            admins.erase(it);
            break ;
        }
    }
}


void    Channel::add_client(Client* client)
{
    this->_clients.push_back(client);
}

void    Channel::remove_client(Client* client)
{
        std::vector<Client *>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        Client *tmp = *it;
        if (tmp->getNickname() == client->getNickname())
        {
            _clients.erase(it);
            break;
        }
        it++;
    }
}

size_t  Channel::getVecSize()
{
    return this->_clients.size();
}

Channel::~Channel()
{
    this->_clients.clear();
    this->admins.clear();
}

size_t Channel::getOpVecSize()
{
    return this->admins.size();
}

void    Channel::chsendMsg(std::string msg)
{
    std::vector<Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
        sendMsg((*it)->getFd(), (*it)->getNickname() + " : " + msg + '\n');
}