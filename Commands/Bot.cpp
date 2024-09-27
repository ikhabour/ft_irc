#include "../Server.hpp"

void    Server::Bot(int fd, std::string cmd)
{
    std::vector<std::string> cmds = split_words(cmd);
    if (cmds.size() != 1)
    {
        sendMsg(fd, "Usage : /bot or /BOT \n");
        return ;
    }
    srand (time(NULL));
    int random = rand() % 5;
    if(random == 0)
        sendMsg(fd, "The space between your eyebrows is called your glabella.\n");
    else if(random == 1)
        sendMsg(fd, "The first number to be spelled using the letter \"a\" is \"one thousand.\"\n");
    else if(random == 2)
        sendMsg(fd, "\"Four\" is the only number that is spelled with the same number of letters as itself.\n");
    else if(random == 3)
        sendMsg(fd, "Bats eat enough insects each year to save the United States more than one billion dollars annually in crop damage and pesticides.\n");
    else if(random == 4)
        sendMsg(fd, "Potatoes were the first vegetable to be grown in space.\n");
}