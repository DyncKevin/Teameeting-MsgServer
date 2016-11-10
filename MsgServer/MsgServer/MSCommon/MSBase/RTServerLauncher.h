#ifndef _DRTSERVER_LAUNCHER_H_
#define _DRTSERVER_LAUNCHER_H_

#include <string>
#include <vector>
#include <set>

class RTServerLauncher{
    public:
        RTServerLauncher();
        ~RTServerLauncher();

        int ParseServerCmdsAndLaunch(const std::string& cmd, const std::vector<std::string>& argv);

    private:
        int DoExecvp(const char* file, char* const argv[]);

    private:
};

#endif // _DRTSERVER_LAUNCHER_H_

