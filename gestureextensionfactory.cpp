#include "gestureextensionfactory.h"
#include "mswitchergesture.h"

MCompositeManagerExtension* GestureExtensionFactory::create()
{
    return new MSwitcherGesture();
}

QString GestureExtensionFactory::extensionName()
{
    return QLatin1String("nemogestures");
}

Q_EXPORT_PLUGIN2(nemogestures, GestureExtensionFactory)

