#include "../Server.hpp"

void    Server::list(int fd, std::string cmd)
{
    Client *client = getClient(fd);
    (void)cmd;
    
    sendMsg(fd, RPL_LISTSTART(client->getNickname()));
    for (size_t i = 0; i < _channels.size(); i++)
    {
        size_t visible = _channels[i]->getVecSize();
        std::stringstream ss;
        ss << visible;
        std::string msg = RPL_LIST(client->getNickname(), _channels[i]->getName(), ss.str(), _channels[i]->getTopic());
        sendMsg(fd, msg);
    }
    sendMsg(fd, RPL_LISTEND(client->getNickname()));
}
