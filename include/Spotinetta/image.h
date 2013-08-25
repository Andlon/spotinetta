#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error image_add_ref(sp_image * image) { return sp_image_add_ref(image); }
inline sp_error image_release(sp_image * image) { return sp_image_release(image); }
inline bool image_is_loaded(sp_image * image) { return sp_image_is_loaded(image); }
}

typedef BasicLoadableObject<sp_image, &detail::image_add_ref, &detail::image_release, &detail::image_is_loaded> BasicImage;

class Image : public BasicImage {
public:
    enum class Format {
        Unknown = SP_IMAGE_FORMAT_UNKNOWN,
        JPEG = SP_IMAGE_FORMAT_JPEG
    };

    Image() { }
    explicit Image(sp_image * image, bool increment = true) : BasicImage(image, increment) { }

    QByteArray data() const;
    Format format() const;
};

}

Q_DECLARE_METATYPE(Spotinetta::Image)
Q_DECLARE_METATYPE(Spotinetta::ImageList)
