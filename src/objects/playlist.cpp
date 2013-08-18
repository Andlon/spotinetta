#include <Spotinetta/playlist.h>

namespace Spotinetta {

QString Playlist::name() const
{
    return isValid() ? QString::fromUtf8(sp_playlist_name(handle())) : QString();
}

int Playlist::trackCount() const
{
    return isValid() ? sp_playlist_num_tracks(handle()) : 0;
}

TrackList Playlist::tracks() const
{
    TrackList list;
    for (int i = 0; i < trackCount(); ++i)
        list << trackAt(i);

    return list;
}

}
