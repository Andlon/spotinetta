#include <Spotinetta/artist.h>

namespace Spotinetta {

QString Artist::name() const
{
    return isValid() ? QString::fromUtf8(sp_artist_name(handle())) : QString();
}

ImageId Artist::portrait(ImageSize size) const
{
    return isValid() ? sp_artist_portrait(handle(), static_cast<sp_image_size>(size)) : 0;
}

}
