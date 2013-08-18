#include <Spotinetta/playlist.h>
#include <Spotinetta/user.h>
#include <Spotinetta/track.h>

namespace Spotinetta {

QString Playlist::name() const
{
    return isValid() ? QString::fromUtf8(sp_playlist_name(handle())) : QString();
}

User Playlist::owner() const
{
    return isValid() ? User(sp_playlist_owner(handle())) : User();
}

int Playlist::trackCount() const
{
    return isValid() ? sp_playlist_num_tracks(handle()) : 0;
}

Track Playlist::trackAt(int index) const
{
    return isValid() ? Track(sp_playlist_track(handle(), index)) : Track();
}

TrackList Playlist::tracks() const
{
    TrackList list;
    for (int i = 0; i < trackCount(); ++i)
        list << trackAt(i);

    return list;
}

}
