#ifndef _SIGNAL_HPP_
#define _SIGNAL_HPP_

#include <csignal>
#include <functional>
#include <unordered_map>

namespace net {

class Signal
{
public:
    typedef std::unordered_map<int, std::function<void()> > Handlers_T;
    static void Register(int SigNo_F, const std::function<void()>& Handler_F)
    {
        m_umapHdlers[SigNo_F] = Handler_F;
        signal(SigNo_F, signal_handler);
    }

    static void signal_handler(int SigNo_F)
    {
        m_umapHdlers[SigNo_F]();
    }

private:
    static Handlers_T m_umapHdlers;
};

Signal::Handlers_T Signal::m_umapHdlers;

} // namespace net

#endif /* _SIGNAL_HPP_ */