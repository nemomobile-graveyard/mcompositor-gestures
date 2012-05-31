#include <QApplication>
#include <QRectF>
#include <QSize>
#include <QX11Info>
#include <QWidget>
#include <QDesktopWidget>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "mainwindow.h"

void disableCompositorAnimations(QWidget *window,  QRectF *rect)
{
   if (window != NULL)
   {
       unsigned int customRegion[] =
       {
           rect->x(),
           rect->y(),
           rect->width(),
           rect->height()
       };

        Display *dpy = QX11Info::display();
        Atom customRegionAtom = XInternAtom(dpy,"_MEEGOTOUCH_CUSTOM_REGION", False);

        XChangeProperty(dpy, window->winId(), customRegionAtom,
                        XA_CARDINAL, 32, PropModeReplace,
                        reinterpret_cast<unsigned char*>(&customRegion[0]), 4);
   }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    QRectF mainWindowRect;
    QSize windowSize;

    windowSize.setWidth( ((QApplication::desktop())->screenGeometry()).width() );
    windowSize.setHeight( ((QApplication::desktop())->screenGeometry()).height() );

    mainWindow.resize( windowSize );
    mainWindow.showFullScreen();
    mainWindowRect.setRect( mainWindow.x(), mainWindow.y(), mainWindow.width(), mainWindow.height() );
    disableCompositorAnimations( &mainWindow,  &mainWindowRect);
    return app.exec();
}
