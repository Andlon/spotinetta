#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error artistbrowse_add_ref(sp_artistbrowse * artistbrowse) { return sp_artistbrowse_add_ref(artistbrowse); }
inline sp_error artistbrowse_release(sp_artistbrowse * artistbrowse) { return sp_artistbrowse_release(artistbrowse); }
inline bool artistbrowse_is_loaded(sp_artistbrowse * artistbrowse) { return sp_artistbrowse_is_loaded(artistbrowse); }
}

typedef BasicLoadableObject<sp_artistbrowse, &detail::artistbrowse_add_ref,
                            &detail::artistbrowse_release,
                            &detail::artistbrowse_is_loaded>
                            BasicArtistBrowse;

class ArtistBrowse : public BasicArtistBrowse {
public:
    ArtistBrowse() { }
    explicit ArtistBrowse(sp_artistbrowse * browse, bool increment = true)
        :   BasicArtistBrowse(browse, increment) { }

    Artist  artist() const;
    QString biography() const;
    Error   error() const;

    int albumCount() const;
    Album albumAt(int index) const;

    int similarArtistCount() const;
    Artist similarArtistAt(int index) const;

    int tophitTrackCount() const;
    Track tophitTrackAt(int index) const;

    int portraitCount() const;
    ImageId portraitAt(int index) const;

    int backendRequestDuration() const;
};

}

Q_DECLARE_METATYPE(Spotinetta::AlbumBrowse)
