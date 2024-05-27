#include <iostream>
#include "camera_HAL.h"
#include "ControlThread.h"

using std::cout;
using std::cin;

static ControlThread *control_thread;

int camera_open(void)
{
    control_thread = new ControlThread;
    return 0;
}

int camera_take_picture(void)
{
    control_thread->takePicture();
    return 0;
}

int camera_close(void)
{
    delete control_thread;
    return 0;
}