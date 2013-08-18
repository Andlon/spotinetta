#include <Spotinetta/playlistcontainer.h>
#include <Spotinetta/playlist.h>
#include <Spotinetta/user.h>

namespace Spotinetta {

int PlaylistContainer::playlistCount() const
{
    return isValid() ? sp_playlistcontainer_num_playlists(handle()) : 0;
}

Playlist PlaylistContainer::playlistAt(int index) const
{
    return isValid() ? Playlist(sp_playlistcontainer_playlist(handle(), index)) : Playlist();
}

PlaylistList PlaylistContainer::playlists() const
{
    PlaylistList list;
    for (int i = 0; i < playlistCount(); ++i)
        list << playlistAt(i);

    return list;
}

User PlaylistContainer::owner() const
{
    return isValid() ? User(sp_playlistcontainer_owner(handle())) : User();
}

}
