#include "../inc/ps_proxy.hpp"

using namespace std;

Proxy::Proxy(string srcAddress, unsigned srcPort, string dstAddress, unsigned dstPort)
    : _srcAddress(srcAddress), _srcPort(srcPort), _dstAddress(dstAddress), _dstPort(dstPort) {}

Proxy::~Proxy() {}

sockaddr_in Proxy::getDstSockaddr()
{
    static sockaddr_in dstSockaddr = getSockaddr(_dstAddress, _dstPort);
    return dstSockaddr;
}
sockaddr_in Proxy::getSrcSockaddr()
{
    static sockaddr_in srcSockaddr = getSockaddr(_srcAddress, _srcPort);
    return srcSockaddr;
}

Proxy Proxy::load(istream& in)
{
    string srcAddress;
    unsigned srcPort;
    string dstAddress;
    unsigned dstPort;

    in >> srcAddress >> srcPort >> dstAddress >> dstPort;

    return Proxy(srcAddress, srcPort, dstAddress, dstPort);
}

void Proxy::save(ostream& out)
{
    out << _srcAddress << ' ' << _srcPort << ' ' << _dstAddress << ' ' << _dstPort << endl;
}

sockaddr_in Proxy::getSockaddr(string ip, unsigned port)
{
    sockaddr_in address;

    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.data(), &(address.sin_addr.s_addr));
    address.sin_port = htons((uint16_t)port);

    return address;
}
