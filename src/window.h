/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "cpustats.h"

#include <memory>
#include <vector>
#include <xcb/xcb.h>

class CWindow final
{
public:
    CWindow(const int size);
    ~CWindow();

    bool Open(const char* className, const char* instance = 0);
    void Close();
    void EventLoop();

private:
    double getTime();
    void cleanPixmap();
    void forceUpdateWindow();
    static void* fnThread(void* p);

private:
    int m_size;
    std::vector<xcb_rectangle_t> m_rcCPU;
    std::vector<int> m_data;
    uint16_t m_width;
    uint16_t m_height;
    bool m_quit;
    xcb_connection_t* m_c;
    xcb_screen_t* m_s;
    xcb_window_t m_w;
    xcb_gcontext_t m_g;
    std::auto_ptr<CCpuStats> m_stat;
    xcb_pixmap_t m_pixmapId;
};
