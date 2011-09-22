#ifndef SWIPEMANAGEREXTENSIONFACTORY
#define SWIPEMANAGEREXTENSIONFACTORY

#include <meegotouch/mcompositor/mcompositemanagerextension.h>
#include <meegotouch/mcompositor/mcompmgrextensionfactory.h>

class SwipeManagerExtensionFactory : public QObject, MCompmgrExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(MCompmgrExtensionFactory)
 public:
    virtual ~SwipeManagerExtensionFactory() {}

    MCompositeManagerExtension *create();
    QString extensionName();
};

#endif // SWIPEMANAGEREXTENSIONFACTORY
