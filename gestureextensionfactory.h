#ifndef GESTUREEXETNSIONFACTORY_H
#define GESTUREEXETNSIONFACTORY_H

#include <meegotouch/mcompositor/mcompositemanagerextension.h>
#include <meegotouch/mcompositor/mcompmgrextensionfactory.h>

class GestureExtensionFactory : public QObject, MCompmgrExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(MCompmgrExtensionFactory)
 public:
    virtual ~GestureExtensionFactory() {}

    MCompositeManagerExtension *create();
    QString extensionName();
};

#endif // GESTUREEXETNSIONFACTORY_H
