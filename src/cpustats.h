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
    CCpuStats();
    ~CCpuStats();

    int getCurrentCPUload(float dt);

private:
    int getCPUcount();

private:
    int m_cpus; // total CPU's count
    int m_prev[9];
};
