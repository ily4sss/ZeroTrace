#include "database.hpp"

DataBase::DataBase()
{
    db = nullptr;
}
DataBase::~DataBase()
{
    if (db != nullptr)
        sqlite3_close(db);
}
bool DataBase::open(const char *file)
{
    int f_op = sqlite3_open(file, &db);
    if (f_op)
    {
        std::cout << "Error: " << sqlite3_errmsg(db);
        return false;
    }
    else
        std::cout << "Successful";
    return true;
}

bool DataBase::CreateUserTable()
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL"
        ");";

    char *err = nullptr;
    int result = sqlite3_exec(db, sql, NULL, 0, &err);
    if (result)
    {
        sqlite3_free(err);
        std::cout << "Error: " << sqlite3_errmsg(db);
        return false;
    }
    return true;

}
bool CheckUserExist(const std::string& user)
{
    const char *sql =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";
    sqlite3_stmt *stmt = nullptr;
    
    int prep = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (prep  != SQLITE_OK)
    {
        std::cout << "Prepare error: "<< sqlite3_errmsg(db) << '\n';
        return false;
    }
    sqlite3_bind_text(stmt, 1, "ilyas", -1, SQLITE_TRANSIENT);
    bool u_exist = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)   
        u_exist = true;
    sqlite3_finalize(stmt);


    return true;    
}  
