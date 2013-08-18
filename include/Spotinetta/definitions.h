#pragma once

#include <QMetaType>
#include <QList>
#include <QString>
#include <libspotify/api.h>

namespace Spotinetta {

class Track;
class Album;
class Artist;
class Playlist;
class PlaylistContainer;
class Search;
class ArtistBrowse;
class AlbumBrowse;
class Image;
class Session;

typedef QList<Track>    TrackList;
typedef QList<Album>    AlbumList;
typedef QList<Artist>   ArtistList;
typedef QList<Playlist> PlaylistList;
typedef QList<Search>   SearchList;
typedef QList<Image>    ImageList;

// The following image enum are not placed in Image,
// because in libspotify they are not directly associated with
// the image object.

enum class ImageSize {
    Small = SP_IMAGE_SIZE_SMALL,
    Normal = SP_IMAGE_SIZE_NORMAL,
    Lage = SP_IMAGE_SIZE_LARGE
};

}
