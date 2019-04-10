/* 
 * File:   ts_core.hpp
 * Author: bogdan
 */

#ifndef TS_CORE_HPP
#define TS_CORE_HPP

#include <functional>
#include <vector>
#include <memory>
#include <tuple>

#include "ts_socket.hpp"
#include "ts_server.hpp"

using namespace std;

typedef shared_ptr<Socket> socket_p;
typedef function<string(const string&, const string&)> processor_t;

extern processor_t execute_action;
extern bool alive;

void register_actions(Server& server);

string command_switcher(const Server& server, const string& command, const string& args);

tuple<string, string> parse_command(const string& request);

void listener(Server server);

#endif /* TS_CORE_HPP */

