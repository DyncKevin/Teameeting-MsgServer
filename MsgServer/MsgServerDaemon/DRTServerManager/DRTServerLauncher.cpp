#include "DRTServerLauncher.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "RTUtils.hpp"
#include "rtklog.h"

DRTServerLauncher::DRTServerLauncher()
{

}

DRTServerLauncher::~DRTServerLauncher()
{

}

int DRTServerLauncher::ParseServerCmdsAndLaunch(const std::string& cmd, const std::vector<std::string>& argv)
{
    if (cmd.length()==0 || cmd.c_str() == nullptr || argv.size()==0) return -1;
    char* arg[8] = {nullptr};
    int i = 0;
    for (;i<argv.size();i++)
    {
        arg[i] = (char*)argv[i].c_str();
        LI("argi:%s\n", arg[i]);
    }
    arg[++i] = nullptr;
    return DoExecvp(cmd.c_str(), arg);
}

////////////////////////////////////////////////////
/////////////////////private////////////////////////
////////////////////////////////////////////////////
int DRTServerLauncher::DoExecvp(const char* file, char* const argv[])
{
    LI("DRTServerLauncher::DoExecvp getpid:%u\n", getpid());
    {
        LI("execvp was called...getpid:%u\n", getpid());
        //umask(0);
        //setsid();
        //chdir("/");
        if (execvp(file, argv)<0)
        {
            fprintf(stderr, "DoExecvp call execvp failed:%s\n", strerror(errno));
            return -2;
        }
    }
    LI("DRTServerLauncher::DoExecvp was called, will return this is last..getpid:%u.\n", getpid());
    return 0;
}
