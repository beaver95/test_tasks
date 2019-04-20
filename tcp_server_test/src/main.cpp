/* 
 * File:   main.cpp
 * Author: bogdan
 */

#include <string>
#include <unistd.h>
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>

#include "../inc/ts_core.hpp"
#include "../inc/ts_socket.hpp"
#include "../inc/ts_server.hpp"

using namespace std;

int main(int argc, char** argv)
{
    using namespace std::placeholders;
    ifstream input("server.conf", ifstream::in);

    Server server = Server::load(input);
    clog << "Configuration details" << endl;
    clog << "Address          - " << server.srcAddress() << ":" << server.srcPort() << endl;
    clog << "Max. connections - " << server.maxConnections() << endl;
    clog << "EOL type         - " << int(server.eolType()) << endl << endl;

    register_actions(server);
    execute_action = bind(command_switcher, ref(server), _1, _2);

    thread listener_th(listener, server);

    cout << "Press 'exit' to close..." << endl;
    string request;
    do {
        cout << "> ";
        getline(cin, request);
        if (execute_action)
        {
            string command, args;
            string eol = eoltype2str(server.eolType());
            tie(command, args) = parse_command(request + eol, eol);
            cout << execute_action(command, args) << endl;
        }
    } while (request.compare("exit") != 0);
    alive = false;

    listener_th.join();

    return 0;
}

