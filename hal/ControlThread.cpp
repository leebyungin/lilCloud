#include <iostream>
#include "ControlThread.h"
#include "message.h"
using std::cin;
using std::cout;

ControlThread::ControlThread()
{
    //cout<< "ControlThread() (from C++)\n";
    pMessage("ControlThread() (from C++)\n");
}

ControlThread::~ControlThread()
{
    //cout<< "~ControlThread() (from C++)\n";
    pMessage("~ControlThread() (from C++)\n");
}

int ControlThread::takePicture()
{
    //cout<< "takePicture() (from C++)\n";
    pMessage("takePicture() (from C++)\n");
    return 0;
}