/*
* Copyright (C) 2011 Robin Burchell <robin+nemo@viroteck.net>
*
* You may use this file under the terms of the BSD license as follows:
*
* "Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* * Neither the name of Nemo Mobile nor the names of its contributors
* may be used to endorse or promote products derived from this
* software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
*/

#include <QDebug>
#include <QApplication>
#include <QX11Info>
#include <QDesktopWidget>

#include <meegotouch/mcompositor/mcompositemanager.h>

#include "mswitchergesture.h"

#include <X11/Xlib.h>

// define to enable debug logging
#undef SWITCHER_DEBUG

// XXX: need to free this probably

MSwitcherGesture::MSwitcherGesture()
    : startX(-1)
    , startY(-1)
    , swiping(false)
{
    listenXEventType(GenericEvent);


    // check for extension
    int event, error;
    if (!XQueryExtension(QX11Info::display(), "XInputExtension", &opcode, &event, &error))
        qFatal("MSwitcherGesture: X Input extension not available.\n");

    // and make sure we have a good version
    int major = 2, minor = 0;
    if (XIQueryVersion(QX11Info::display(), &major, &minor) == BadRequest)
        qFatal("MSwitcherGesture: XI2 not available. Server supports %d.%d\n", major, minor);

    // copied from Qt, with thanks
    // find the first master pointer and use this throughout Qt
    // when making XI2 calls that need a device id (rationale is that
    // for the time being, most setups will only have one master
    // pointer (despite having multiple slaves)
    int deviceCount = 0;
    XIDeviceInfo *devices = XIQueryDevice(QX11Info::display(), XIAllMasterDevices, &deviceCount);
    if (devices) {
        for (int i = 0; i < deviceCount; ++i) {
            if (devices[i].use == XIMasterPointer) {
                int unused = 0;
                xideviceinfo = XIQueryDevice(QX11Info::display(), devices[i].deviceid, &unused);
                break;
            }
        }
        XIFreeDeviceInfo(devices);
    }
    if (!xideviceinfo)
        qFatal("MSwitcherGesture: Internal error, no XI2 master pointer found.");

    // thanks to Daniel Stone for his help with learning the tricks of XInput2
    XIEventMask xieventmask;
    xieventmask.mask_len = XIMaskLen(XI_LASTEVENT);
    xieventmask.deviceid = xideviceinfo->deviceid;
    xieventmask.mask = (unsigned char *)calloc(xieventmask.mask_len, sizeof(char));

    XISetMask(xieventmask.mask, XI_ButtonPress);
    XISetMask(xieventmask.mask, XI_ButtonRelease);
    XISetMask(xieventmask.mask, XI_Motion); 

    // TODO: error checking, etc
    XIGrabModifiers mods { XIAnyModifier, 0 };
    XIGrabButton(QX11Info::display(),   // dpy
            xideviceinfo->deviceid, // deviceid
            XIAnyButton, // detail
            RootWindow(QX11Info::display(), 0), // grab_window
            None, // cursor
            GrabModeSync, // grab_mode
            GrabModeSync, // paired_device_mode
            False, // owner_events
            &xieventmask, // mask
            1, // num_modifiers
            &mods // modifiers_inout
            );  

    free(xieventmask.mask);
}


/*!
 * Special event handler to receive native X11 events passed in the event
 * parameter. Return true to stop the composite manager from handling
 * the event, otherwise return false to try forwarding the native event to 
 * the composite manager.
 * 
 * If there are other extensions that are subscribed to the same event and
 * this function returns true, the event will still be blocked from
 * propagating to the composite manager even if this function
 * returns false. Be careful when returning true when there are other
 * extensions around and only use as a last resort to reimplement core
 * functionality.
 */
bool MSwitcherGesture::x11Event(XEvent *event)
{
    if (event->type == GenericEvent) {
        bool keepGrab = false;

        if (event->xcookie.extension == opcode &&
            (event->xcookie.evtype == XI_ButtonPress ||
             event->xcookie.evtype == XI_Motion ||
             event->xcookie.evtype == XI_ButtonRelease)) {
            XIDeviceEvent *xievent = (XIDeviceEvent *)event->xcookie.data;
#if defined(SWITCHER_DEBUG)
//            qDebug() << Q_FUNC_INFO << "Got XIEvent type " << xievent->type;
#endif

            if (event->xcookie.evtype == XI_Motion) {
                keepGrab = onMousePositionChanged(xievent->root_x, xievent->root_y);
            } else if (event->xcookie.evtype == XI_ButtonPress) {
                keepGrab = onPressed(xievent->root_x, xievent->root_y);
            } else if (event->xcookie.evtype == XI_ButtonRelease) {
                keepGrab = onReleased(xievent->root_x, xievent->root_y);
            }
        }


        if (keepGrab) {
#if defined(SWITCHER_DEBUG)
            qDebug() << Q_FUNC_INFO << "Keeping grab";
#endif
            XIAllowEvents(QX11Info::display(), xideviceinfo->deviceid, SyncPointer, CurrentTime);
        } else {
#if defined(SWITCHER_DEBUG)
            qDebug() << Q_FUNC_INFO << "Releasing grab";
#endif
            XIAllowEvents(QX11Info::display(), xideviceinfo->deviceid, ReplayPointer, CurrentTime);
        }
    }
    return false;
}

void MSwitcherGesture::afterX11Event(XEvent *event)
{

}

bool MSwitcherGesture::onPressed(int x, int y)
{
    const int allowedSwipeWidth = 20;
    const int windowWidth = QApplication::desktop()->width(); // XXX: should we query window width, or desktop width here?
    const int windowHeight = QApplication::desktop()->height();

    if (x <= allowedSwipeWidth || windowWidth - allowedSwipeWidth < x) {
#if defined(SWITCHER_DEBUG)
        qDebug() << Q_FUNC_INFO << "Swipe started on an X edge";
#endif
        startX = x;
        return true;
    } else if (y <= allowedSwipeWidth || windowHeight - allowedSwipeWidth < y) {
#if defined(SWITCHER_DEBUG)
        qDebug() << Q_FUNC_INFO << "Swipe started on a Y edge";
#endif
        startY = y;
        return true;
    }

    return false;
}

bool MSwitcherGesture::onReleased(int x, int y)
{
    bool retval = false;

    if (swiping == true) {
        const int diffX = abs(startX - x);
        const int diffY = abs(startY - y);
        const int windowWidth = QApplication::desktop()->width(); // XXX: should we query window width, or desktop width here?
        const int windowHeight = QApplication::desktop()->height();
        const float cancelShortEdgeSwipe = 0.5;
        const float cancelLongEdgeSwipe = 0.25;

#if defined(SWITCHER_DEBUG)
        if (diffX > diffY) {
            // horizontal swipe
            if (windowWidth * cancelShortEdgeSwipe < diffX)
                qDebug() << Q_FUNC_INFO << "Swipe ended at " << x << y << "; was a swipe on X";
        } else {
            // vertical swipe
            if (windowHeight * cancelLongEdgeSwipe < diffY)
                qDebug() << Q_FUNC_INFO << "Swipe ended at " << x << y << "; was a swipe on Y";
        }
#endif

        MCompositeManager *manager = qobject_cast<MCompositeManager *>(qApp);
        if (!manager)
            qFatal("MSwitcherGesture: not running in mcompositor!?");
        manager->exposeSwitcher();
        retval = true;
    }

    swiping = false;
    startX = -1;
    startY = -1;

    return retval;
}

bool MSwitcherGesture::onMousePositionChanged(int x, int y)
{
    if (swiping == true)
        return true;
  
    const int swipeThreshold = 20;
    
    if((0 <= startX && swipeThreshold < abs(x - startX)) || 
       (0 <= startY && swipeThreshold < abs(y - startY)) ) {
        swiping = true;
#if defined(SWITCHER_DEBUG)
        qDebug() << Q_FUNC_INFO << "Swipe started at " << x << y;
#endif
    }

    return true;
}

