PROJECT_NAME = gesture-lock-testapp
QT += declarative
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

target.path = $$INSTALL_ROOT/usr/bin
INSTALLS += target

desktop.files = $${PROJECT_NAME}.desktop
desktop.path = $$INSTALL_ROOT/usr/share/applications
INSTALLS += desktop

LIBS += -lX11

# Input
SOURCES += main.cpp mainwindow.cpp

HEADERS += mainwindow.h
