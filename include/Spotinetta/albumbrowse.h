#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error albumbrowse_add_ref(sp_albumbrowse * albumbrowse) { return sp_albumbrowse_add_ref(albumbrowse); }
inline sp_error albumbrowse_release(sp_albumbrowse * albumbrowse) { return sp_albumbrowse_release(albumbrowse); }
inline bool albumbrowse_is_loaded(sp_albumbrowse * albumbrowse) { return sp_albumbrowse_is_loaded(albumbrowse); }
}

typedef BasicLoadableObject<sp_albumbrowse, &detail::albumbrowse_add_ref,
                            &detail::albumbrowse_release,
                            &detail::albumbrowse_is_loaded>
                            BasicAlbumBrowse;

class AlbumBrowse : public BasicAlbumBrowse {
public:
    AlbumBrowse() { }
    explicit AlbumBrowse(sp_albumbrowse * browse, bool increment = true)
        :   BasicAlbumBrowse(browse, increment) { }

    Album album() const;
    Artist artist() const;
    QString review() const;
    Error error() const;
    int backendRequestDuration() const;

    int copyrightCount() const;
    QString copyrightAt(int index) const;

    int trackCount() const;
    Track trackAt(int index) const;
};

}

Q_DECLARE_METATYPE(Spotinetta::AlbumBrowse)
