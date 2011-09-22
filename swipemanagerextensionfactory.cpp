#include "swipemanagerextensionfactory.h"

MCompositeManagerExtension* SwipeManagerExtensionFactory::create()
{
    return new MSwipeExtension();
}

QString SwipeManagerExtensionFactory::extensionName()
{
    return QLatin1String("opensourceswipe");
}

Q_EXPORT_PLUGIN2(meeswipe, SwipeManagerExtensionFactory)

