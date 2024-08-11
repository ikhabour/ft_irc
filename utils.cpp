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

bool    ValidChannelName(std::string chname)
{
    if (chname.empty())
        return false;
    else if ((chname[0] == '#' || chname[0] == '&') && !isalphanumeric(chname.substr(1)))
        return false;
    else if (chname.length() == 1 && (chname[0] == '&' || chname[0] == '#'))
        return false;
    else if (chname[0] != '#' && chname[0] != '&')
        return false;
    return true;
}

bool    isvalidMode(std::string mode)
{
    if (mode.length() < 2 || mode.length() > 2)
        return false;
    if (mode[0] == '+' || mode[0] == '-')
    {
        if (mode[1] == 'l' || mode[1] == 't' || mode[1] == 'i' || mode[1] == 'k' || mode[1] == 'o')
            return true;
        else
            return false;
    }
    return false;
}

bool    requiresParam(std::string mode)
{
    if (mode == "+o" || mode == "-o" || mode == "+k" || mode == "+l")
        return true;
    return false;
}

bool    isNumeric(std::string param)
{
    for (size_t i = 0; i < param.size(); i++)
    {
        if (!isdigit(param[i]))
            return false;
    }
    return true;
}
