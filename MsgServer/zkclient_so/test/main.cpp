#include "ZKClt.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char** argv)
{
    printf("hello\n");
    if (argc <2) return 0;
    printf("argv1 is:%s\n", argv[1]);
    sleep(3);
    ZKClt c;
    printf("c.DoUpload 1...\n");
    sleep(3);
    if (!c.Init(argv[1]))
    {
        return 0;
    }
    printf("c.DoUpload 2...\n");
    sleep(3);
    while(1)
    {
        printf("c.DoUpload...\n");
        sleep(3);
        c.DoUpload();
    }
    return 0;
}
