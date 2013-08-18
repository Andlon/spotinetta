#include <Spotinetta/user.h>

namespace Spotinetta {

QString User::canonicalName() const
{
    return isValid() ? QString::fromUtf8(sp_user_canonical_name(handle())) : QString();
}

QString User::displayName() const
{
    return isValid() ? QString::fromUtf8(sp_user_display_name(handle())) : QString();
}

}
