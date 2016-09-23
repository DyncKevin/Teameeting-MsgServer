#include "UtilTimer.h"

#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

bool UtilTimer::mRun = false;
OnWorkers UtilTimer::mWk = NULL;

UtilTimer::UtilTimer()
{
}

UtilTimer::~UtilTimer()
{
}

void UtilTimer::Init(OnWorkers wk)
{
     mWk = wk;
     mRun = true;
     InitSigac();
     InitTimer();
     while(mRun);
}

void UtilTimer::InitSigac(void)
{
    struct sigaction tact;
    tact.sa_handler = &UtilTimer::Process;
    tact.sa_flags = 0;
    sigemptyset(&tact.sa_mask);
    sigaction(SIGALRM, &tact, NULL);
}

void UtilTimer::InitTimer()
{
     struct itimerval value;
     value.it_value.tv_sec = 1;
     value.it_value.tv_usec = 0;
     value.it_interval = value.it_value;
     setitimer(ITIMER_REAL, &value, NULL);
}
void UtilTimer::StopTimer()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_REAL, &value, NULL);
}

void UtilTimer::Process(int)
{
    if (mWk)
    {
        if (mWk()==0)
        {
            mRun = false;
            StopTimer();
        }
    }
}
