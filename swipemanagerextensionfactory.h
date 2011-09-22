#ifndef SWIPEMANAGEREXTENSIONFACTORY
#define SWIPEMANAGEREXTENSIONFACTORY

#include <meegotouch/mcompositor/mcompmgrextensionfactory.h>

class SwipeManagerExtensionFactory : public MCompmgrExtensionFactory
{
    Q_INTERFACES(MCompmgrExtensionFactory)
 public:
    virtual ~SwipeManagerExtensionFactory() {}

    virtual MCompositeManagerExtension* create();
    {
        return new MSwipeExtension();
    }

    virtual QString extensionName();
    {
        return QLatin1String("opensourceswipe");
    }
};

#endif // SWIPEMANAGEREXTENSIONFACTORY
