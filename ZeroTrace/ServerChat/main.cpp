#include "library.h"
#include "server.hpp"
int main()
{
    Server server(5555);
    server.Create();
    server.Bind();
    server.Listen();
    server.Connection();
    return 0;
}
