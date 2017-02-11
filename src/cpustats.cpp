/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "cpustats.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

CCpuStats::CCpuStats()
{
    m_cpus = getCPUcount();
    //  printf("We have %d cpu's:\n", m_cpus);
    memset(m_prev, 0, sizeof(m_prev));
}

CCpuStats::~CCpuStats()
{
}

int CCpuStats::getCurrentCPUload(float dt)
{
    int result = 0;

    FILE* f = fopen("/proc/stat", "r");
    if (f != nullptr)
    {
        char buf[200];
        while (fgets(buf, sizeof(buf), f))
        {
            int cur[9];
            int count = sscanf(buf, "cpu %d %d %d %d %d %d %d %d %d\n", &cur[0], &cur[1], &cur[2], &cur[3], &cur[4], &cur[5], &cur[6], &cur[7], &cur[8]);
            if (count == 9)
            {
                int tmp[9];
                for (size_t i = 0; i < 9; i++)
                {
                    //printf("%.2f ", cur[i]);
                    if (m_prev[i] > 0)
                    {
                        tmp[i] = static_cast<int>((cur[i] - m_prev[i]) / dt);
                    }
                    else
                    {
                        tmp[i] = 0;
                    }
                    m_prev[i] = cur[i];
                }
                //printf("\n");

                result = (tmp[0] + tmp[1] + tmp[2] + tmp[4] + tmp[5] + tmp[6]) / m_cpus;
                break;
            }
        }

        fclose(f);
    }

    return result;
}

int CCpuStats::getCPUcount()
{
    //from http://www.cplusplus.com/forum/unices/6544/
    char buffer[128] = { 0 };
    FILE* f = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
    if (f != nullptr)
    {
        auto result = fread(buffer, 1, sizeof(buffer) - 1, f);
        (void)result;
        pclose(f);

        if (buffer[0] != 0)
        {
            return atoi(buffer);
        }
    }

    return 1;
}
