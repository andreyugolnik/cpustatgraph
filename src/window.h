/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "cpustats.h"
#include "ringbuffer.h"

#include <memory>
#include <vector>
#include <xcb/xcb.h>

class CWindow final
{
public:
    bool Open(const char* className, const char* instance = 0);
    void Close();
    void EventLoop();

private:
    double getTime();
    void cleanPixmap();
    void forceUpdateWindow();
    static void* fnThread(void* p);

private:
    cRingBuffer<int, 80> m_data;
    CCpuStats m_stat;
    uint16_t m_width = m_data.size();
    uint16_t m_height = 16;
    bool m_quit = false;
    xcb_connection_t* m_c;
    xcb_screen_t* m_s;
    xcb_window_t m_w;
    xcb_gcontext_t m_g;
    xcb_pixmap_t m_pixmapId;
};
