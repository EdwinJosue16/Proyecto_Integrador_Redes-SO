#ifndef TIMESTAMP
#define TIMESTAMP


#include <ctime>
class Timestamp
{
    private:
        time_t myTime;
        double sencodsToExpire;
    public:

        ///Default constructor
        Timestamp()
        {
            time(&myTime);
            sencodsToExpire=0;
        }
        
        Timestamp(const Timestamp &);

        ///Upload the timestamp with actual local time
        inline void upload()
        {
            time(&myTime);
        }

        ///Return time of timestamp
        inline time_t getTime()
        {
            time_t copy=myTime;
            return copy;
        }

        ///Set time to timestamp
        inline void setTime(time_t newTime)
        {
            myTime=newTime;
        }

        ///Set time to expire (in seconds) to timestamp
        inline void setSecodsToExpire(double newSeconds)
        {
            sencodsToExpire=newSeconds;
        }

        void operator=(const Timestamp &);

        ///Determine if the timestamp expired (needs actual time for determining)
        bool expired(Timestamp & actual);
};
#endif