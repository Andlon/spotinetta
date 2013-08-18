#include <Spotinetta/track.h>
#include <Spotinetta/artist.h>
#include <Spotinetta/album.h>
#include <Spotinetta/session.h>

namespace Spotinetta {

QString Track::name() const
{
    return isValid() ? QString::fromUtf8(sp_track_name(handle())) : QString();
}

int Track::duration() const
{
    return isValid() ? sp_track_duration(handle()) : 0;
}

int Track::popularity() const
{
    return isValid() ? sp_track_popularity(handle()) : 0;
}

int Track::artistCount() const
{
    return isValid()? sp_track_num_artists(handle()) : 0;
}

Artist Track::artistAt(int index) const
{
    return isValid() ? Artist(sp_track_artist(handle(), index)) : Artist();
}

ArtistList Track::artists() const
{
    ArtistList list;
    for (int i = 0; i < artistCount(); ++i)
        list << artistAt(i);

    return list;
}

Album Track::album() const
{
    return isValid() ? Album(sp_track_album(handle())) : Album();
}

bool Track::isAutoLinked(const Session * session) const
{
    if (isValid() && session->isValid())
    {
        return sp_track_is_autolinked(session->handle(), handle());
    }

    return false;
}

Track::Availability Track::availability(const Session * session) const
{
    if (isValid() && session->isValid())
    {
        return static_cast<Availability>(sp_track_get_availability(session->handle(), handle()));
    }

    return Availability::Unavailable;
}

Track Track::playableTrack(const Session * session) const
{
    if (isValid() && session->isValid())
    {
        return Track(sp_track_get_playable(session->handle(), handle()));
    }

    return Track();
}



}
