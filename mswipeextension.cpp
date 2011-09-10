#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

#include "mswipeextension.h"

#include <X11/Xlib.h>

MSwipeExtension::MSwipeExtension()
    : startX(-1)
    , startY(-1)
    , swiping(false)
    , lockSwipe(false)
{
    listenXEventType(ButtonPress);
    listenXEventType(ButtonRelease);
    listenXEventType(MotionNotify);
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
bool MSwipeExtension::x11Event(XEvent *event)
{
    // TODO: return values
    if (event->type == ButtonPress) {
        XButtonPressedEvent *buttonPressedEvent = reinterpret_cast<XButtonPressedEvent *>(event);
        onPressed(buttonPressedEvent);
        return false;
    } else if (event->type == ButtonRelease) {
        XButtonReleasedEvent *buttonReleaseEvent = reinterpret_cast<XButtonReleasedEvent *>(event);
        onReleased(buttonReleaseEvent);
        return false;
    } else if (event->type == MotionNotify) {
        XMotionEvent *motionEvent = reinterpret_cast<XMotionEvent *>(event);
        onMousePositionChanged(motionEvent);
        return false;
    }
}

void MSwipeExtension::afterX11Event(XEvent *event)
{

}

bool MSwipeExtension::onPressed(XButtonPressedEvent *event)
{
    int x = event->x_root;
    int y = event->y_root;
    qDebug() << Q_FUNC_INFO << "pressed at " << x << y;

    const int allowedSwipeWidth = 20;
    const int windowWidth = QApplication::desktop()->width(); // XXX: should we query window width, or desktop width here?
    const int windowHeight = QApplication::desktop()->height();

    if (x <= allowedSwipeWidth || windowWidth - allowedSwipeWidth < x) {
        qDebug() << Q_FUNC_INFO << "Swipe started on an X edge";
        startX = x;
        return true;
    } else if (y <= allowedSwipeWidth || windowHeight - allowedSwipeWidth < y) {
        qDebug() << Q_FUNC_INFO << "Swipe started on a Y edge";
        startY = y;
        return true;
    }

    return false;
}

bool MSwipeExtension::onReleased(XButtonPressedEvent *event)
{
    bool retval = false;
    int x = event->x_root;
    int y = event->y_root;

    qDebug() << Q_FUNC_INFO << "released at " << x << y;

    if (swiping == true) {
        const int diffX = abs(startX - x);
        const int diffY = abs(startY - x);
        const int windowWidth = QApplication::desktop()->width(); // XXX: should we query window width, or desktop width here?
        const int windowHeight = QApplication::desktop()->height();
        const float cancelShortEdgeSwipe = 0.5;
        const float cancelLongEdgeSwipe = 0.25;

        if (diffX > diffY) {
            // horizontal swipe
            if (windowWidth * cancelShortEdgeSwipe < diffX) {
                qDebug() << Q_FUNC_INFO << "Was a swipe on X";
            }
        } else {
            // vertical swipe
            if (windowHeight * cancelLongEdgeSwipe < diffY) {
                qDebug() << Q_FUNC_INFO << "Was a swipe on Y";
            }
        }

        qDebug() << Q_FUNC_INFO << "swipe ended";
        retval = true;
    }

    swiping = false;
    startX = -1;
    startY = -1;

    return retval;
}

bool MSwipeExtension::onMousePositionChanged(XMotionEvent *event)
{
    int x = event->x_root;
    int y = event->y_root;
    const int swipeThreshold = 10;

    if((0 <= startX || 0 <= startY) && swiping == false) {
        if ((swipeThreshold < abs(x - startX)) ||
            (swipeThreshold < abs(y - startY))) {
            swiping = true;
            qDebug() << Q_FUNC_INFO << "Swipe started";
        }
    }

    if (swiping == true) {
        // move window
    }
    return false;
}

