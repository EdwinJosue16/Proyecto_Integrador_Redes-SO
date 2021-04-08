#include "Timer.h"

//If a signal is receive before timeout, then return true
bool Timer::putAndWait(uint8_t time)
{
    std::unique_lock<std::mutex> lck(this->padlock);
    return conditionVariable.wait_for(lck,std::chrono::seconds(time))!=std::cv_status::timeout;
}

//Send a signal to the timer in order to break down the wait for method
void Timer::stop()
{
    conditionVariable.notify_one();
}