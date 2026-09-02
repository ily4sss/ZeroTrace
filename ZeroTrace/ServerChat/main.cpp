#include "library.hpp"
#include "server.hpp"
#include "database.hpp"
int main()
{
    // Server server(5555);
    // server.Create();
    // server.Bind();
    // server.Listen();
    // server.Connection();
    DataBase db;
    db.open("zerotrace.db");
    
    return 0;
}
