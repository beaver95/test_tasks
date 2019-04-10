/* 
 * File:   ps_socket.hpp
 * Author: bogdan
 */

#ifndef TS_SOCKET_HPP
#define TS_SOCKET_HPP

using namespace std;

class Socket
{
public:
    Socket();
    Socket(int sock);
    virtual ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    inline int sock() { return _sock; }
    inline int sock() const { return _sock; }
private:
    int _sock;
};

#endif /* TS_SOCKET_HPP */

