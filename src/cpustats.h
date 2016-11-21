/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class CCpuStats final
{
public:
    CCpuStats(int* data, int size);
    ~CCpuStats();

    void Update(float dt);

private:
    int getCurrentCPUload(float dt);
    int getCPUcount();

private:
    int* m_cpuLoad; // array that stores CPU load info
    int m_size;     // array size
    int m_cpus;     // total CPU's count
    int m_prev[9];
};
