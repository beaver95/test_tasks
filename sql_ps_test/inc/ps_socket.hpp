/* 
 * File:   ps_socket.h
 * Author: bogdan
 */

#ifndef PS_SOCKET_HPP
#define PS_SOCKET_HPP

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

#endif /* PS_SOCKET_HPP */

