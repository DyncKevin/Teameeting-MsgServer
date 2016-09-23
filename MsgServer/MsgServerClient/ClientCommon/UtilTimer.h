#ifndef _UTIL_TIMER_H_
#define _UTIL_TIMER_H_

#include <stdio.h>

typedef int (*OnWorkers)();

class UtilTimer{
    public:

        UtilTimer();
        ~UtilTimer();
        void Init(OnWorkers wk);

    private:
        void InitTimer();
        void InitSigac();
        static void Process(int);
        static void StopTimer();
    private:
        static bool mRun;
        static OnWorkers     mWk;
};

#endif // _UTIL_TIMER_H_
