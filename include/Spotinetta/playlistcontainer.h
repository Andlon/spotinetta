#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error playlistcontainer_add_ref(sp_playlistcontainer * playlistcontainer) { return sp_playlistcontainer_add_ref(playlistcontainer); }
inline sp_error playlistcontainer_release(sp_playlistcontainer * playlistcontainer) { return sp_playlistcontainer_release(playlistcontainer); }
inline bool playlistcontainer_is_loaded(sp_playlistcontainer * playlistcontainer) { return sp_playlistcontainer_is_loaded(playlistcontainer); }
}

typedef BasicLoadableObject<sp_playlistcontainer, &detail::playlistcontainer_add_ref, &detail::playlistcontainer_release, &detail::playlistcontainer_is_loaded> BasicPlaylistContainer;

class PlaylistContainer : public BasicPlaylistContainer {
public:
    PlaylistContainer() { }
    PlaylistContainer(sp_playlistcontainer * playlistcontainer, bool increment = true) : BasicPlaylistContainer(playlistcontainer, increment) { }

    User owner() const;

    int playlistCount() const;
    Playlist playlistAt(int index) const;
    PlaylistList playlists() const;
};

}
