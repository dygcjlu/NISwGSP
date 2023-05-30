#include "common.h"

void getNowTv(timeval& tv)
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> m_now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> m_seconds=std::chrono::time_point_cast<std::chrono::seconds>(m_now);
    if(m_seconds>m_now)
    {
        m_seconds = m_seconds-std::chrono::seconds(1);
    }
    std::chrono::duration<long,std::ratio<1,1000000>> m_microseconds=std::chrono::duration_cast<std::chrono::microseconds>(m_now-m_seconds);
    tv.tv_sec=m_seconds.time_since_epoch().count();
    tv.tv_usec=m_microseconds.count();
}
