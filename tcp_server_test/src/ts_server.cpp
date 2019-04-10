#include "../inc/ts_server.hpp"

using namespace std;

Server::Server(string srcAddress, unsigned srcPort, unsigned maxConnections)
    : _srcAddress(srcAddress), _srcPort(srcPort), _maxConnections(maxConnections), _actionCbs() {}

Server::~Server() {}

void Server::registerAction(const string& action_name, ts_action action)
{
    _actionCbs[action_name] = action;
}

ts_action Server::getAction(const string& action_name) const
{
    auto action = _actionCbs.find(action_name);
    if (action != _actionCbs.end())
    {
        return action->second;
    }

    return 0;
}

sockaddr_in Server::getSrcSockaddr()
{
    static sockaddr_in srcSockaddr = getSockaddr(_srcAddress, _srcPort);
    return srcSockaddr;
}

Server Server::load(istream& in)
{
    string srcAddress = "0.0.0.0";
    unsigned srcPort = 5678;
    unsigned maxConnections = 3;

    in >> srcAddress >> srcPort >> maxConnections;

    return Server(srcAddress, srcPort, maxConnections);
}

void Server::save(ostream& out)
{
    out << _srcAddress << ' ' << _srcPort << ' ' << _maxConnections << endl;
}

sockaddr_in Server::getSockaddr(string ip, unsigned port)
{
    sockaddr_in address;

    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.data(), &(address.sin_addr.s_addr));
    address.sin_port = htons((uint16_t)port);

    return address;
}
