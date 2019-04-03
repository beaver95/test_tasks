/* 
 * File:   ps_core.hpp
 * Author: bogdan
 */

#ifndef PS_CORE_HPP
#define PS_CORE_HPP

#include <functional>
#include <vector>
#include <memory>

#include "ps_socket.hpp"
#include "ps_proxy.hpp"
#include "ps_writer.hpp"

using namespace std;

typedef shared_ptr<Socket> socket_p;
typedef function<void(const unsigned char*, int)> processor_t;

extern vector<processor_t> processors;
extern bool alive;

void printSqlQuery(const unsigned char* str, int len);

void listener(Proxy proxy);

#endif /* PS_CORE_HPP */

