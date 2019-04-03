#include "../inc/ps_core.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <algorithm>

using namespace std;

static map<socket_p, socket_p> connections;
static mutex connection_mtx;

vector<processor_t> processors;
bool alive = true;

void printSqlQuery(const unsigned char* str, int len)
{
    unsigned pack_len = (unsigned)str[0] | (unsigned)str[1] << 8 | (unsigned)str[2] << 16;
    unsigned pack_num = str[3];
    if (pack_len == 0 || pack_num != 0)
        return;

    int cmd_type = str[4];
    static vector<int> cmd_types = {3, 22};
    if (!binary_search(cmd_types.begin(), cmd_types.end(), cmd_type))
        return;

    string query ((char*)&str[5], pack_len - 1);
    Writer::get().print(query);
    return;
}

static void processor()
{
    clog << "processing started" << endl;
    unsigned char buff[4096];
    int rval;

    while (!connections.empty())
    {
        
        connection_mtx.lock();
        map<socket_p, socket_p> connections_ro(connections);
        connection_mtx.unlock();
        for (const pair<socket_p, socket_p>& connection : connections_ro)
        {
            socket_p client, server;
            tie(client, server) = connection;
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
                for (processor_t process : processors)
                {
                    process(buff, rval);
                }

                if (send(server->sock(), buff, rval, 0) < 0)
                {
                    clog << "Send failed" << endl;
                    continue;
                }
            }
        }
    }
    clog << "processing stopped" << endl;
}

void listener(Proxy proxy)
{
    shared_ptr<thread> processor_thread;
    socket_p sock = make_shared<Socket>();
    unsigned char buff[4096];
    int timeout = 1000;

    sockaddr_in server = proxy.getSrcSockaddr();

    int flags = fcntl(sock->sock(), F_GETFL, 0);
    fcntl(sock->sock(), F_SETFL, flags | O_NONBLOCK);

    if (bind(sock->sock(), (sockaddr*)&server, sizeof(server)))
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
            timeout = min(1000, timeout + 10);
            if (errno != EAGAIN)
            {
                cerr << "Accept failed" << endl;
            }
        }
        else
        {
            timeout/=10;
            socket_p mysock = make_shared<Socket>(accepted_sock);

            clog << "Accept. sock: " << mysock->sock() << ", port: " << ntohs(client.sin_port) << endl;
            //memset(buff, 0, sizeof(buff));

            socket_p sqlsock = make_shared<Socket>();
            sockaddr_in sqlserver = proxy.getDstSockaddr();

            if (connect(sqlsock->sock(), (sockaddr*)&sqlserver, sizeof(sqlserver)) < 0)
            {
                cerr << "Connect failed" << endl;
                continue;
            }

            connection_mtx.lock();
            connections[mysock] = sqlsock;
            connections[sqlsock] = mysock;
            connection_mtx.unlock();
        }

        if (processor_thread == nullptr && !connections.empty())
        {
            processor_thread = make_shared<thread>(processor);
        }

        //clog << "timeout: " << timeout << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));
    } while(alive);

    if (processor_thread != nullptr)
    {
        connections.clear();
        processor_thread->join();
    }
}
