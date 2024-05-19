#pragma once


#include "Server.hpp"

class Client;

class Channel
{
    private :
        std::string _name;
        std::string _password;
        std::string _topic;
        bool _i;
        bool _t;
        int _l;
        std::vector<Client *> _clients;
        std::vector<Client *> admins;
    public :
        Channel(std::string& chname, std::string &key);
        ~Channel();


        /*    Setters     */


        void setName(std::string& Name);
        void setPass(std::string Password);
        void setLimit(int lim);
        void setTopic(std::string& Topic);
        void restrictTopic(bool status);
        void setInviteOnly(bool status);
        void setOperator(Client* client);
        void removeOperator(std::string nickname);

        /*    Getters     */
        
        std::string getName() const;
        std::string getPass() const;
        std::string getTopic() const;
        int getLimit() const;
        bool    getInviteStatus();
        bool    getTopicStatus();
        size_t  getVecSize();
        int isOperator(Client* client);
        size_t getOpVecSize();
        Client* find_client(std::string username);


        /*      Others      */

        void    add_client(Client* client);
        void    remove_client(Client* client);
        void    assignNextOp();

        void    PrintOperators();
};