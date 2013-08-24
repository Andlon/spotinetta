#include <Spotinetta/albumbrowse.h>
#include <Spotinetta/album.h>
#include <Spotinetta/artist.h>
#include <Spotinetta/track.h>

#include <QStringList>

namespace Spotinetta {

Album AlbumBrowse::album() const
{
    return isValid() ? Album(sp_albumbrowse_album(handle())) : Album();
}

Artist AlbumBrowse::artist() const
{
    return isValid() ? Artist(sp_albumbrowse_artist(handle())) : Artist();
}

QString AlbumBrowse::review() const
{
    return isValid() ? QString::fromUtf8(sp_albumbrowse_review(handle())) : QString();
}

Error AlbumBrowse::error() const
{
    return isValid() ? static_cast<Error>(sp_albumbrowse_error(handle())) : Error::Ok;
}

int AlbumBrowse::backendRequestDuration() const
{
    return isValid() ? sp_albumbrowse_backend_request_duration(handle()) : 0;
}

int AlbumBrowse::copyrightCount() const
{
    return isValid() ? sp_albumbrowse_num_copyrights(handle()) : 0;
}

QString AlbumBrowse::copyrightAt(int index) const
{
    return isValid() ? QString::fromUtf8(sp_albumbrowse_copyright(handle(), index)) : QString();
}

QStringList AlbumBrowse::copyrights() const
{
    QStringList list;
    int count = copyrightCount();
    for (int i = 0; i < count; ++i)
        list << copyrightAt(i);

    return list;
}

int AlbumBrowse::trackCount() const
{
    return isValid() ? sp_albumbrowse_num_tracks(handle()) : 0;
}

Track AlbumBrowse::trackAt(int index) const
{
    return isValid() ? Track(sp_albumbrowse_track(handle(), index)) : Track();
}

TrackList AlbumBrowse::tracks() const
{
    TrackList list;
    int count = trackCount();
    for (int i = 0; i < count; ++i)
        list << trackAt(i);

    return list;
}

}
