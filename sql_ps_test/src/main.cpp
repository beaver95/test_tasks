/* 
 * File:   main.cpp
 * Author: bogdan
 */

#include <string>
#include <unistd.h>
#include <thread>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

#include "../inc/ps_core.hpp"
#include "../inc/ps_socket.hpp"
#include "../inc/ps_proxy.hpp"
#include "../inc/ps_writer.hpp"

using namespace std;

int main(int argc, char** argv)
{
    ifstream input("proxy.conf", ifstream::in);
    ofstream sqllog("sql.log", ofstream::out);

    Proxy proxy = Proxy::load(input);
    Writer::get().start(sqllog);

    processors.push_back(printSqlQuery);

    thread listener_th(listener, proxy);

    cout << "Press 'exit' to close..." << endl;
    string any;
    do {
        cout << "> ";
        getline(cin, any);
    } while (any.compare("exit") != 0);
    alive = false;

    listener_th.join();
    Writer::get().stop();

    return 0;
}

