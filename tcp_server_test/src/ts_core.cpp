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

static string response_msg;
static bool exit_needed;

processor_t execute_action;
bool alive = true;

static bool action_exit(const string& args)
{
    if (args.empty())
    {
        exit_needed = true;
        return true;
    }
    return false;
}

static bool action_version(const string& args)
{
    if (args.empty())
    {
        response_msg += "Version 0.1";
        return true;
    }
    return false;
}

void register_actions(Server& server)
{
    server.registerAction("exit", action_exit);
    server.registerAction("version", action_version);
}

string command_switcher(const Server& server, const string& command, const string& args)
{
    exit_needed = false;
    response_msg = "";

    ts_action action = server.getAction(command);
    return (action && action(args)) ? "OK" : "ERROR";
}

tuple<string, string> parse_command(const string& request, const string& eol)
{
    string command, args, eolStr;
    eolStr = eol.size() ? eol : "\n";
    string request_str = string() + "^([^ ]+)( ([^" + eolStr[0] + "]+))?" + eolStr + "$";
    regex request_line(request_str);
    smatch match;
    if (regex_match(request, match, request_line))
    {
        clog << "command : " << match[1] << "." << endl;
        command = match[1];
        clog << "args    : " << match[3] << "." << endl;
        args = match[3];
    }
    else
    {
        clog << "Parsing failed: " << request << endl;
    }
    return tuple<string, string>(command, args);
}

static void processor(const Server& server)
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

                tie(command, args) = parse_command(request, eoltype2str(server.eolType()));

                if (execute_action)
                {
                    response = execute_action(command, args);
                    clog << "response: " << response << "." << endl;
                }
                else
                {
                    clog << "execute_action was not registered" << endl;
                }


                if (exit_needed)
                {
                    connection_mtx.lock();
                    connections.erase(client);
                    connection_mtx.unlock();
                    clog << "Ending connection" << endl;
                }
                else
                {
                    if (!response_msg.empty())
                    {
                        response_msg += eoltype2str(server.eolType());
                        if (send(client->sock(), response_msg.data(), response_msg.size(), 0) < 0)
                        {
                            clog << "Send failed" << endl;
                        }
                    }

                    response += eoltype2str(server.eolType());
                    if (send(client->sock(), response.data(), response.size(), 0) < 0)
                    {
                        clog << "Send failed" << endl;
                    }
                }
            }
        }
    }
    clog << "processing stopped" << endl;
}

void listener(const Server& server)
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
            processor_thread = make_shared<thread>(processor, server);
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
