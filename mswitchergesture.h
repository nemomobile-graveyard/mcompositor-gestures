#ifndef MSWITCHERGESTURE_H
#define MSWITCHERGESTURE_H

#include <QObject>

#include <meegotouch/mcompositor/mcompositemanagerextension.h>

#include <X11/extensions/XInput2.h>

class MSwitcherGesture : public MCompositeManagerExtension
{
    Q_OBJECT
public:
    MSwitcherGesture();

    bool x11Event(XEvent *event);
    void afterX11Event(XEvent *event);

    bool onPressed(int x, int y);
    bool onReleased(int x, int y);
    bool onMousePositionChanged(int x, int y);

private:
    int startX;
    int startY;
    bool swiping;

    // TODO: do we need to follow device changes?
    XIDeviceInfo *xideviceinfo; // TODO: need to free this?
    int opcode;
};

#endif // MSWITCHERGESTURE_H
