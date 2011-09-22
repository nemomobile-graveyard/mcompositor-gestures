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

    bool onPressed(XButtonPressedEvent *event);
    bool onReleased(XButtonPressedEvent *event);
    bool onMousePositionChanged(XMotionEvent *event);

private:
    int startX;
    int startY;
    bool swiping;
    bool lockSwipe;
};

#endif // MSWIPEEXTENSION_H
