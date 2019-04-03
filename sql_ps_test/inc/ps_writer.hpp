/* 
 * File:   ps_writer.hpp
 * Author: bogdan
 */

#ifndef PS_WRITER_HPP
#define PS_WRITER_HPP

#include <string>
#include <thread>
#include <iostream>
#include <queue>
#include <mutex>

using namespace std;

class Writer
{
public:
    virtual ~Writer();
    static Writer& get();

    Writer(Writer const&) = delete;
    Writer& operator=(Writer const&) = delete;

    void start(ostream& ost);
    void stop();

    void print(string& msg);

protected:
    Writer();

    inline ostream& getsout() { return *sout; }

    void writer();

private:
    bool alive;
    mutex mtx;
    queue<string> msgQueue;
    thread writerTh;
    ostream* sout;
};

#endif /* PS_WRITER_HPP */

