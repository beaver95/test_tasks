/* 
 * File:   ts_server.hpp
 * Author: bogdan
 */

#ifndef TS_SERVER_HPP
#define TS_SERVER_HPP

#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <functional>
#include <iomanip>

using namespace std;

typedef function<bool(const string&)> ts_action;

class Server
{
public:
    Server(string srcAddress, unsigned srcPort, unsigned maxConnections);
    virtual ~Server();

    inline string srcAddress() const { return _srcAddress; }
    inline unsigned srcPort() const { return _srcPort; }
    inline unsigned maxConnections() const { return _maxConnections; }

    void registerAction(const string& action_name, ts_action action);

    ts_action getAction(const string& action_name) const;
    sockaddr_in getSrcSockaddr();

    static Server load(istream& in);
    void save(ostream& out);

protected:

    sockaddr_in getSockaddr(string ip, unsigned port);

private:
    string _srcAddress;
    unsigned _srcPort;
    unsigned _maxConnections;
    map<string, ts_action> _actionCbs;
};

#endif /* TS_SERVER_HPP */

