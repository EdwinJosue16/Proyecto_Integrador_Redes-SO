#ifndef TIMER
#define TIMER
#include <mutex>              //std::mutex, std::unique_lock
#include <condition_variable> //std::condition_variable, std::cv_status
#include <chrono>             //std::chrono::seconds
class Timer 
{
    private:
        std::mutex padlock;
        std::condition_variable conditionVariable;
    public:
        Timer()
        {
        }
        ~Timer()
        {
        }
        bool putAndWait(uint8_t time);
        void stop();
};
#endif