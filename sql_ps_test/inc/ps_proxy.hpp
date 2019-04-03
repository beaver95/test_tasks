/* 
 * File:   ps_proxy.hpp
 * Author: bogdan
 */

#ifndef PS_PROXY_HPP
#define PS_PROXY_HPP

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <iomanip>

using namespace std;

class Proxy
{
public:
    Proxy(string srcAddress, unsigned srcPort, string dstAddress, unsigned dstPort);
    virtual ~Proxy();

    inline string srcAddress() const { return _srcAddress; }
    inline unsigned srcPort() const { return _srcPort; }
    inline string dstAddress() const { return _dstAddress; }
    inline unsigned dstPort() const { return _dstPort; }

    sockaddr_in getDstSockaddr();
    sockaddr_in getSrcSockaddr();

    static Proxy load(istream& in);
    void save(ostream& out);

protected:

    sockaddr_in getSockaddr(string ip, unsigned port);

private:
    string _srcAddress;
    unsigned _srcPort;
    string _dstAddress;
    unsigned _dstPort;
};

#endif /* PS_PROXY_HPP */

