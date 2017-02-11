/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "window.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

bool CWindow::Open(const char* className, const char* instance)
{
    if (instance == nullptr)
    {
        instance = className;
    }
    int len_class = strlen(className);
    int len_instance = strlen(instance);

    // open connection with the server
    m_c = xcb_connect(0, 0);
    if (xcb_connection_has_error(m_c))
    {
        printf("Cannot open display\n");
        return false;
    }
    // get the first screen
    m_s = xcb_setup_roots_iterator(xcb_get_setup(m_c)).data;

    // create black graphics context
    m_g = xcb_generate_id(m_c);
    uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    uint32_t values[2] = { m_s->white_pixel, 0 };
    xcb_create_gc(m_c, m_g, m_s->root, mask, values);

    // create window
    m_w = xcb_generate_id(m_c);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = m_s->white_pixel; //m_s->black_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_RESIZE_REDIRECT | XCB_EVENT_MASK_BUTTON_PRESS;//XCB_EVENT_MASK_KEY_PRESS;
    xcb_create_window(m_c, XCB_COPY_FROM_PARENT, m_w, m_s->root, 0, 0, m_width, m_height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, m_s->root_visual, mask, values);

    // set the title of the window
    xcb_change_property(m_c, XCB_PROP_MODE_REPLACE, m_w, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, len_class, className);

    // set the title of the window icon
    xcb_change_property(m_c, XCB_PROP_MODE_REPLACE, m_w, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8, len_class, className);

    // instance + class
    int len = len_instance + len_class + 2;
    std::vector<char> pci(len, 0);
    sprintf(&pci[0], "%s %s", instance, className);
    pci[len_instance] = 0;
    xcb_change_property(m_c, XCB_PROP_MODE_REPLACE, m_w, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8, len, &pci[0]);

    // map (show) the window
    xcb_map_window(m_c, m_w);

    xcb_flush(m_c);

    // {
    // uint32_t values[2] = { m_width, m_height };
    // xcb_configure_window(m_c, m_w, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    // }

    // // get actual window size
    // xcb_get_geometry_reply_t* geom = xcb_get_geometry_reply(m_c, xcb_get_geometry(m_c, m_w), nullptr);
    // printf("Actual window size: %d x %d, %d x %d\n", geom->x, geom->y, geom->width, geom->height); fflush(stdout);
    // m_height = geom->height;
    // free(geom);

    // create pixmap
    m_pixmapId = xcb_generate_id(m_c);
    xcb_create_pixmap(m_c,
                      m_s->root_depth, // depth of the screen
                      m_pixmapId,      // id of the pixmap
                      m_w,
                      m_width,         // pixel width of the window
                      m_height);       // pixel height of the window

    cleanPixmap();

    return true;
}

void CWindow::Close()
{
    xcb_free_pixmap(m_c, m_pixmapId);

    // close connection to server
    xcb_disconnect(m_c);
}

void CWindow::EventLoop()
{
    pthread_t thread;
    int rc;
    if ((rc = pthread_create(&thread, nullptr, &fnThread, this)))
    {
        printf("Thread creation failed: %d\n", rc);
    }



    // (width+7) / 8 * height

    // event loop
    xcb_generic_event_t* e;
    while (m_quit == false && (e = xcb_wait_for_event(m_c)))
    {
        switch (e->response_type & ~0x80)
        {
        case XCB_EXPOSE: // draw or redraw the window
            // copy pixmap on window
            xcb_copy_area(m_c,
                          m_pixmapId, // drawable we want to paste
                          m_w,        // drawable on which we copy the previous Drawable
                          m_g,
                          0,          // top left x coordinate of the region we want to copy
                          0,          // top left y coordinate of the region we want to copy
                          0,          // top left x coordinate of the region where we want to copy
                          0,          // top left y coordinate of the region where we want to copy
                          m_width,    // pixel width of the region we want to copy
                          m_height);  // pixel height of the region we want to copy

            xcb_flush(m_c);
            break;

        case XCB_RESIZE_REQUEST:
        {
            // get actual window size
            xcb_get_geometry_reply_t* geom = xcb_get_geometry_reply(m_c, xcb_get_geometry(m_c, m_w), nullptr);
            m_height = geom->height;
            free(geom);
            // force setup desired size of window
            uint32_t values[2] = { m_width, m_height };
            xcb_configure_window(m_c, m_w, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
        }
        break;

        case XCB_BUTTON_PRESS: //XCB_KEY_PRESS:  // exit on key press
            m_quit = true;
            break;
        }
        free(e);
    }

    pthread_join(thread, nullptr);
}

double CWindow::getTime()
{
    timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec + t.tv_usec * 0.0000001;
}

void CWindow::cleanPixmap()
{
    uint32_t val[] = { 0x00000030, 0 };
    xcb_change_gc(m_c, m_g, XCB_GC_FOREGROUND, val);

    xcb_rectangle_t rect = { 0, 0, m_width, m_height };
    xcb_poly_fill_rectangle(m_c, m_pixmapId, m_g, 1, &rect);
}

void CWindow::forceUpdateWindow()
{
    cleanPixmap();

    uint32_t val[] = { 0x0000ff00 };
    xcb_change_gc(m_c, m_g, XCB_GC_FOREGROUND, val);

    // prepare pixmap
    const auto size = m_data.size();
    std::vector<xcb_rectangle_t> rc(size);
    for (size_t i = 0; i < size; i++)
    {
        int h = (int)(m_height * m_data.get((i + 1) % size) * 0.01f);
        rc[i].x = i;
        rc[i].y = m_height - h;
        rc[i].width = 1;
        rc[i].height = m_height;
    }
    xcb_poly_fill_rectangle(m_c, m_pixmapId, m_g, size, rc.data());

    // send exposure event
    static xcb_expose_event_t ev;
    ev.response_type = XCB_EXPOSE;
    ev.window = m_w;
    ev.x      = 0;
    ev.y      = 0;
    ev.count  = 1;
    ev.width  = m_width;
    ev.height = m_height;
    xcb_send_event(m_c, true, m_w, XCB_EVENT_MASK_EXPOSURE, (char*)&ev);
    xcb_flush(m_c);
}

void* CWindow::fnThread(void* p)
{
    auto& w = *static_cast<CWindow*>(p);

    auto start = w.getTime();
    const auto interval = 1.0;

    while (w.m_quit == false)
    {
        usleep(100000);

        const auto now = w.getTime();
        const auto dt = now - start;
        if (dt >= interval)
        {
            const auto value = w.m_stat.getCurrentCPUload(dt);
            w.m_data.addValue(value);

            w.forceUpdateWindow();
            start = now;
        }
    }

    return nullptr;
}
