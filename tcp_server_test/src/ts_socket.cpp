#include "../inc/ts_socket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

Socket::Socket() : _sock(-1)
{
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    clog << "Creating socket " << _sock << endl;
}

Socket::Socket(int sock) : _sock(sock)
{
    clog << "Creating socket " << _sock << endl;
}

Socket::~Socket()
{
    clog << "Closing socket " << _sock << endl;
    if (_sock != -1)
    {
        close(_sock);
    }
}
