/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "window.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>

int main()
{
    printf("CPU Statistic History v0.1.4 by Andrey A. Ugolnik\n");
    printf("Copyright (c) 2009, 2010, 2015\n");
    printf("   http://www.ugolnik.info\n");
    printf("   mailto:andrey@ugolnik.info\n");

    // detach from terminal
    int pid = fork();
    if (pid)
    {
        // parent, return
        return 0;
    }

    CWindow win(80);
    if (win.Open("CpuStatGraph", "cpustatgraph") == false)
    {
        return -1;
    }

    win.EventLoop();
    win.Close();

    return 0;
}
