#ifndef DATABASE
#define DATABASE

#include"library.hpp"

#include<sqlite3.h>

class DataBase
{  
private:
    sqlite3 *db;
public :
    DataBase();
    ~DataBase();

    bool open(const char *file);
    bool CreateUserTable();
    bool CheckUserExist(std:string& user);
}; 
#endif