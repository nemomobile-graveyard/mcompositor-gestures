/*
* Copyright (C) 2011 Robin Burchell <robin+nemo@viroteck.net>
*               2012 Marko Saukko <marko.saukko@gmail.com>
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

#ifndef MSWITCHERGESTURE_H
#define MSWITCHERGESTURE_H

#include <QRegion>
#include <QObject>

#include <meegotouch/mcompositor/mcompositemanagerextension.h>

#include <X11/extensions/XInput2.h>

class MSwitcherGesture : public MCompositeManagerExtension
{
    Q_OBJECT
public:
    MSwitcherGesture();
    ~MSwitcherGesture();

    bool x11Event(XEvent *event);
    void afterX11Event(XEvent *event);

    bool onPressed(int x, int y);
    bool onReleased(int x, int y);
    bool onMousePositionChanged(int x, int y);

private slots:
    void appWindowChanged(Qt::HANDLE window);
    
private:
    // This function is used to get the custom region that is used to block gestures.
    // More information can be found from the Harmattan documentation:
    // http://harmattan-dev.nokia.com/docs/library/html/guide/html/Developer_Library_Developing_for_Harmattan_Enabling_swipe_lock_Example_of_a_manual_swipe_lock.html
    bool getCustomRegion(Qt::HANDLE window, QRegion& customRegion);
    
    int startX;
    int startY;
    bool swiping;

    // TODO: do we need to follow device changes?
    XIDeviceInfo *xideviceinfo;
    int opcode;
    
    Qt::HANDLE currentAppWindow;
};

#endif // MSWITCHERGESTURE_H
