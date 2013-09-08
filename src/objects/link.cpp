#include <Spotinetta/link.h>
#include <Spotinetta/track.h>
#include <Spotinetta/artist.h>
#include <Spotinetta/album.h>
#include <Spotinetta/image.h>
#include <Spotinetta/artistbrowse.h>

namespace Spotinetta {

Link::Link(const QString &uri)
    :   BasicLink(sp_link_create_from_string(uri.toUtf8()), false)
{ }

Link::Link(const Artist &artist)
    :   BasicLink(sp_link_create_from_artist(artist.handle()), false)
{ }

Link::Link(const Album &album)
    :   BasicLink(sp_link_create_from_album(album.handle()), false)
{ }

Link::Link(const Track &track, int offset)
    :   BasicLink(sp_link_create_from_track(track.handle(), offset), false)
{ }

Link::Link(const Image &image)
    :   BasicLink(sp_link_create_from_image(image.handle()), false)
{ }

Link::Type Link::type() const
{
    return isValid() ? static_cast<Link::Type>(sp_link_type(handle())) : Link::Type::Invalid;
}

QString Link::uri() const
{
    if (isValid())
    {
        char buffer[100];
        int count = sp_link_as_string(handle(), buffer, 100);
        return QString::fromUtf8(buffer, count);
    }

    return QString();
}

Track Link::track() const
{
    return isValid() ? Track(sp_link_as_track(handle())) : Track();
}

Artist Link::artist() const
{
    return isValid() ? Artist(sp_link_as_artist(handle())) : Artist();
}

Album Link::album() const
{
    return isValid() ? Album(sp_link_as_album(handle())) : Album();
}

Link Link::fromAlbumCover(const Album &album, ImageSize size)
{
    return album.isValid() ?
                Link(sp_link_create_from_album_cover(album.handle(), static_cast<sp_image_size>(size)))
              : Link();
}

Link Link::fromArtistPortrait(const Artist &artist, ImageSize size)
{
    return artist.isValid() ?
                Link(sp_link_create_from_artist_portrait(artist.handle(), static_cast<sp_image_size>(size)))
              : Link();
}

Link Link::fromArtistBrowsePortrait(const ArtistBrowse &browse, int index)
{
    return browse.isValid() ?
                Link(sp_link_create_from_artistbrowse_portrait(browse.handle(), index))
              : Link();

}

}
