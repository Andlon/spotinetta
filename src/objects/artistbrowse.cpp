#include <Spotinetta/artistbrowse.h>

#include <Spotinetta/artist.h>
#include <Spotinetta/album.h>
#include <Spotinetta/track.h>

namespace Spotinetta {

Artist ArtistBrowse::artist() const
{
    return isValid() ? Artist(sp_artistbrowse_artist(handle())) : Artist();
}

QString ArtistBrowse::biography() const
{
    return isValid() ? QString::fromUtf8(sp_artistbrowse_biography(handle())) : QString();
}

Error ArtistBrowse::error() const
{
    return isValid() ? static_cast<Error>(sp_artistbrowse_error(handle())) : Error::Ok;
}

int ArtistBrowse::albumCount() const
{
    return isValid() ? sp_artistbrowse_num_albums(handle()) : 0;
}

Album ArtistBrowse::albumAt(int index) const
{
    return isValid() ? Album(sp_artistbrowse_album(handle(), index)) : Album();
}

int ArtistBrowse::similarArtistCount() const
{
    return isValid() ? sp_artistbrowse_num_similar_artists(handle()) : 0;
}

Artist ArtistBrowse::similarArtistAt(int index) const
{
    return isValid() ? Artist(sp_artistbrowse_similar_artist(handle(), index)) : Artist();
}

int ArtistBrowse::tophitTrackCount() const
{
    return isValid() ? sp_artistbrowse_num_tophit_tracks(handle()) : 0;
}

Track ArtistBrowse::tophitTrackAt(int index) const
{
    return isValid() ? Track(sp_artistbrowse_tophit_track(handle(), index)) : Track();
}

int ArtistBrowse::portraitCount() const
{
    return isValid() ? sp_artistbrowse_num_portraits(handle()) : 0;
}

ImageId ArtistBrowse::portraitAt(int index) const
{
    return isValid() ? sp_artistbrowse_portrait(handle(), index) : 0;
}

int ArtistBrowse::backendRequestDuration() const
{
    return isValid() ? sp_artistbrowse_backend_request_duration(handle()) : 0;
}

}
