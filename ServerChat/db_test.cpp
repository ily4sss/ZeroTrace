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
        std::cout << "Error\n";
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
        std::cout << "Error\n";
        return false;
    }
    return true;

}
bool DataBase::CheckUserExist(const std::string& user)
{
    const char *sql =
        "SELECT 1 FROM users WHERE username = ? LIMIT 1;";
    sqlite3_stmt *stmt = nullptr;
    
    int prep = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (prep  != SQLITE_OK)
    {
        std::cout << "Prepare error\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    bool u_exist = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)   
        u_exist = true;
    sqlite3_finalize(stmt);


    return true;    
}  
bool DataBase::AddUser(const  std::string& user)
{
    const char *sql =
        "INSERT INTO users (username) VALUES (?);";
    sqlite3_stmt *stmt = nullptr;
    int prep = sqlite3_prepare_v2(db, sql, -1 , &stmt , nullptr);
    if (prep  != SQLITE_OK)
    {
        std::cout << "Prepare error\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    int add_user = sqlite3_step(stmt);
    if (add_user != SQLITE_DONE)
    {
        std::cout << "Inseration error\n";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}
bool DataBase::CreateMessagesTable()
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sender TEXT NOT NULL,"
        "receiver TEXT NOT NULL,"
        "content TEXT NOT NULL"
        ");";

    char *err = nullptr;
    int result = sqlite3_exec(db, sql, NULL, 0, &err);
    if (result)
    {
        sqlite3_free(err);
        std::cout << "Error\n";
        return false;
    }
    return true;
}


bool DataBase::SaveMessage(const std::string& sender,const std::string& receiver,const std::string& content)
{
    const char *sql =
        "INSERT INTO messages (sender, receiver, content)"
        "VALUES (?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;
    int prep = sqlite3_prepare_v2(db, sql, -1 , &stmt , nullptr);
    if (prep  != SQLITE_OK)
    {
        std::cout << "Prepare error\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_TRANSIENT);
    int add_user = sqlite3_step(stmt);
    if (add_user != SQLITE_DONE)
    {
        std::cout << "Inseration error\n";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

std::vector<s_message> DataBase::GetHistory(const std::string& sender,const std::string& receiver)
{
    const char *sql =
        "SELECT sender, receiver, content "
        "FROM messages "
        "WHERE "
        "(sender = ? AND receiver = ?) "
        "OR "
        "(sender = ? AND receiver = ?) "
        "ORDER BY id ASC;";

    sqlite3_stmt *stmt = nullptr;
    int prep = sqlite3_prepare_v2(db, sql, -1 , &stmt , nullptr);
    if (prep  != SQLITE_OK)
    {
        std::cout << "Prepare error\n";
    }
    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 3, receiver.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, sender.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<s_message> history;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        s_message message;

        message.sender = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));

        message.recv = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

        message.content = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

        history.push_back(message);
    }

    sqlite3_finalize(stmt);
    
    return history;
}