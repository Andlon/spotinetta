#pragma once

#include <QMetaType>
#include <QList>
#include <QString>
#include <libspotify/api.h>

#include "error.h"

namespace Spotinetta {

typedef const byte * ImageId;

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
class Link;
class User;

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
    Large = SP_IMAGE_SIZE_LARGE
};

void registerTypes();

}
