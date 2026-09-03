#ifndef DATABASE
#define DATABASE

#include "library.hpp"
#include <sqlite3.h>

class DataBase
{  
private:
    sqlite3 *db;
public :
    DataBase();
    ~DataBase();

    bool open(const char *file);
    bool CreateUserTable();
    bool CheckUserExist(const std::string& user);
    bool AddUser(const std::string & user);
    bool CreateMessagesTable();
    bool SaveMessage(const std::string& sender,const std::string& receiver,const std::string& content);
    std::vector<s_message> GetHistory(const std::string& sender,const std::string& receiver);
};
#endif