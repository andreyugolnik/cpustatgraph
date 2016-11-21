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
#include <iostream>

CCpuStats::CCpuStats(int* data, int size)
    : m_cpuLoad(data)
    , m_size(size)
{
    m_cpus = getCPUcount();
    //  printf("We have %d cpu's:\n", m_cpus);
    memset(m_prev, 0, sizeof(m_prev));
}

CCpuStats::~CCpuStats()
{
}

void CCpuStats::Update(float dt)
{
    for (int i = m_size - 1; i > 0; i--)
    {
        assert(i - 1 >= 0);
        m_cpuLoad[i] = m_cpuLoad[i - 1];
    }
    m_cpuLoad[0] = getCurrentCPUload(dt);
}

int CCpuStats::getCurrentCPUload(float dt)
{
    int ret = 0;

    FILE* f = fopen("/proc/stat", "r");
    if (f != 0)
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

                ret = (tmp[0] + tmp[1] + tmp[2] + tmp[4] + tmp[5] + tmp[6]) / m_cpus;
                break;
            }
        }

        fclose(f);
    }

    return ret;
}

int CCpuStats::getCPUcount()
{
    //from http://www.cplusplus.com/forum/unices/6544/
    char res[128] = { 0 };
    FILE* f = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
    if (f != 0)
    {
        fread(res, 1, sizeof(res) - 1, f);
        pclose(f);

        if (res[0] != 0)
        {
            return atoi(res);
        }
    }

    return 0;
}
