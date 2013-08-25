#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error playlist_add_ref(sp_playlist * playlist) { return sp_playlist_add_ref(playlist); }
inline sp_error playlist_release(sp_playlist * playlist) { return sp_playlist_release(playlist); }
inline bool playlist_is_loaded(sp_playlist * playlist) { return sp_playlist_is_loaded(playlist); }
}

typedef BasicLoadableObject<sp_playlist, &detail::playlist_add_ref, &detail::playlist_release, &detail::playlist_is_loaded> BasicPlaylist;

class Playlist : public BasicPlaylist {
public:
    Playlist() { }
    explicit Playlist(sp_playlist * playlist, bool increment = true) : BasicPlaylist(playlist, increment) { }

    QString name() const;
    User owner() const;

    int trackCount() const;
    Track trackAt(int index) const;
    TrackList tracks() const;

};

}

Q_DECLARE_METATYPE(Spotinetta::Playlist)
Q_DECLARE_METATYPE(Spotinetta::PlaylistList)
