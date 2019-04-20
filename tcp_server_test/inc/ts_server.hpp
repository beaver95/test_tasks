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

enum class EndOfLineType : int {
    CR = 0,
    LF,
    CRLF,
};

string eoltype2str (EndOfLineType eol);

class Server
{
public:
    Server(string srcAddress, unsigned srcPort, unsigned maxConnections, EndOfLineType eolType);
    virtual ~Server();

    inline string srcAddress() const { return _srcAddress; }
    inline unsigned srcPort() const { return _srcPort; }
    inline unsigned maxConnections() const { return _maxConnections; }
    inline EndOfLineType eolType() const { return _eolType; }

    void registerAction(const string& action_name, ts_action action);

    ts_action getAction(const string& action_name) const;
    sockaddr_in getSrcSockaddr() const;

    static Server load(istream& in);
    void save(ostream& out);

protected:

    sockaddr_in getSockaddr(string ip, unsigned port) const;

private:
    string _srcAddress;
    unsigned _srcPort;
    unsigned _maxConnections;
    EndOfLineType _eolType;
    map<string, ts_action> _actionCbs;
};

#endif /* TS_SERVER_HPP */

