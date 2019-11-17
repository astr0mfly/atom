#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <iostream>
#include <sstream>
#include <iosfwd>
#include <thread>
#include <string>

#include "..\..\rsc\common\utility.h"
#include "..\..\dfx\comon\debug.h"

namespace dsm {

class Mission : private NoCopy
{
public:
    typedef unsigned long long Ull_T;
    explicit Mission(const Func &Task_F) :
        m_instFunc__(Task_F)
    { }

    virtual ~Mission()
    { 
        if (m_thProc__.native_handle()) {
            logInfo("thread:detach %llu", getIdOfULL());
            free();//析构的时候线程还在，为了不阻塞主线程，detach掉，由INIT进程回收
        }
    }

    inline void start()
    {
        std::thread thTmp(m_instFunc__);
        m_thProc__.swap(thTmp);
    }

    inline void stop()
    {
        m_thProc__.join();
    }

    inline void free()
    {
        if (m_thProc__.native_handle()) {
            m_thProc__.detach();
        }
    }

    std::string getIdOfString()
    {
        return getThreadIdOfString(std::this_thread::get_id());
    }

    Ull_T getIdOfULL()
    {
        return getThreadIdOfULL(std::this_thread::get_id());
    }

    void printThreadId()
    {
        std::thread::id id = std::this_thread::get_id();
        std::cout << "this thread id : " << id << std::endl;
    }

private:
    std::string getThreadIdOfString(const std::thread::id& Id_F)
    {
        std::thread::id instThreadId = std::this_thread::get_id();
        std::stringstream instStrIn;
        instStrIn << instThreadId;
        return instStrIn.str();
    }

    Ull_T getThreadIdOfULL(const std::thread::id& Id_F)
    {
        std::thread::id instThreadId = std::this_thread::get_id();
        return std::stoull(getThreadIdOfString(Id_F));
    }

    Func            m_instFunc__;
    std::thread     m_thProc__;
};

} // namespace dsm

#endif /* _THREAD_HPP_ */