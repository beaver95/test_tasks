#include "../inc/ts_core.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <regex>
#include <set>
#include <mutex>
#include <algorithm>

using namespace std;

static set<socket_p> connections;
static mutex connection_mtx;

processor_t execute_action;
bool alive = true;

static bool action_hello(const string& args)
{
    return args.compare("world") == 0;
}

void register_actions(Server& server)
{
    server.registerAction("hello", action_hello);
}

string command_switcher(const Server& server, const string& command, const string& args)
{
    if (args.empty())
    {
        if (command.compare("exit") == 0)
            return "";
        else if (command.compare("version") == 0)
            return "Version 0.1";
    }

    ts_action action = server.getAction(command);
    return (action(args)) ? "Ok" : "ERROR";
}

tuple<string, string> parse_command(const string& request)
{
    string command, args;
    regex request_line("^([^ ]+)( ([^\n]+))?\n$");
    smatch match;
    if (regex_match(request, match, request_line))
    {
        clog << "command : " << match[1] << "." << endl;
        command = match[1];
        clog << "args    : " << match[3] << "." << endl;
        args = match[3];
    }
    return tuple<string, string>(command, args);
}

static void processor()
{
    clog << "processing started" << endl;
    unsigned char buff[4096];
    int rval;

    while (!connections.empty())
    {
        
        connection_mtx.lock();
        set<socket_p> connections_ro(connections);
        connection_mtx.unlock();
        for (const socket_p& client : connections_ro)
        {
            rval = recv(client->sock(), buff, sizeof(buff), MSG_DONTWAIT);

            if (rval == 0)
            {
                connection_mtx.lock();
                connections.erase(client);
                connection_mtx.unlock();
                clog << "Ending connection" << endl;
            }
            else if (rval < 0)
            {
                if (errno != EAGAIN)
                {
                    cerr << "Read failed" << endl;
                }
            }
            else
            {
                string request((char*)buff, rval);
                string command, args, response;

                tie(command, args) = parse_command(request);

                if (execute_action)
                {
                    response = execute_action(command, args);
                    clog << "response: " << response << "." << endl;
                }
                else
                {
                    clog << "execute_action was not registered" << endl;
                }

                if (!response.empty())
                {
                    response.push_back('\n');
                    if (send(client->sock(), response.data(), response.size(), 0) < 0)
                    {
                        clog << "Send failed" << endl;
                        continue;
                    }
                }
                else
                {
                    connection_mtx.lock();
                    connections.erase(client);
                    connection_mtx.unlock();
                    clog << "Ending connection" << endl;
                }
            }
        }
    }
    clog << "processing stopped" << endl;
}

void listener(Server server)
{
    shared_ptr<thread> processor_thread;
    socket_p sock = make_shared<Socket>();

    sockaddr_in server_addr = server.getSrcSockaddr();

    int flags = fcntl(sock->sock(), F_GETFL, 0);
    fcntl(sock->sock(), F_SETFL, flags | O_NONBLOCK);

    if (bind(sock->sock(), (sockaddr*)&server_addr, sizeof(server_addr)))
    {
        cerr << "Cannot bind to address and port" << endl;
        close(sock->sock());
        return;
    }

    listen(sock->sock(), 5);

    do
    {
        sockaddr_in client;
        socklen_t clientsize = sizeof(client);
        int accepted_sock = accept(sock->sock(), (sockaddr*)&client, &clientsize);


        if (processor_thread != nullptr && connections.empty())
        {
            processor_thread->join();
            processor_thread = nullptr;
        }

        if (accepted_sock == -1)
        {
            if (errno != EAGAIN)
            {
                cerr << "Accept failed" << endl;
            }
        }
        else
        {
            socket_p mysock = make_shared<Socket>(accepted_sock);

            clog << "Accept. sock: " << mysock->sock() << ", port: " << ntohs(client.sin_port) << endl;

            if (server.maxConnections() > connections.size())
            {
                connection_mtx.lock();
                connections.insert(mysock);
                connection_mtx.unlock();
            }
            else
            {
                clog << "Server is busy. Too much connections..." << endl;
            }
        }

        if (processor_thread == nullptr && !connections.empty())
        {
            processor_thread = make_shared<thread>(processor);
        }
    } while(alive);

    if (processor_thread != nullptr)
    {
        connection_mtx.lock();
        connections.clear();
        connection_mtx.unlock();
        processor_thread->join();
    }
}
