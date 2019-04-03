#include "../inc/ps_writer.hpp"

#include <chrono>
#include <iomanip>

using namespace std;

Writer::Writer() : alive(true), writerTh(&Writer::writer, this), sout(&cout) {}
Writer::~Writer() {}
Writer& Writer::get() { static Writer writer; return writer; }

void Writer::start(ostream& ost)
{
    if (alive)
    {
        alive = false;
        writerTh.join();
    }
    alive = true;
    sout = &ost;
    writerTh = thread(&Writer::writer, this);
}

void Writer::stop()
{
    if (alive)
    {
        alive = false;
        writerTh.join();
    }
}

void Writer::print(string& msg)
{
    mtx.lock();
    msgQueue.push(msg);
    mtx.unlock();
}

void Writer::writer()
{
    do
    {
        mtx.lock();
        while (!msgQueue.empty())
        {
            static unsigned msg_num = 0;
            getsout() << "msg(" << setw(10) << ++msg_num << "):\t" << msgQueue.front() << endl;
            msgQueue.pop();
        }
        getsout() << flush;
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    } while(alive);
}
