#ifndef MSWIPEEXTENSION_H
#define MSWIPEEXTENSION_H

#include <QObject>

#include <meegotouch/mcompositor/mcompositemanagerextension.h>

class MSwipeExtension : public MCompositeManagerExtension
{
    Q_OBJECT
public:
    MSwipeExtension();

    bool x11Event(XEvent *event);
    void afterX11Event(XEvent *event);

    bool onPressed(int x, int y);
    bool onReleased(int x, int y);
    bool onMousePositionChanged(int x, int y);

private:
    int startX;
    int startY;
    bool swiping;
    bool lockSwipe;

    // TODO: do we need to follow device changes?
    XIDeviceInfo *xideviceinfo; // TODO: need to free this?
    int opcode;
};

#endif // MSWIPEEXTENSION_H
