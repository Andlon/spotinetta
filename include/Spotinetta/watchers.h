#pragma once

#include "basicwatcher.h"
#include "playlistcontainerwatcher.h"
#include "playlistwatcher.h"

#include "track.h"
#include "artist.h"
#include "album.h"

namespace Spotinetta {

typedef BasicWatcher<Track, &Session::metadataUpdated>     TrackWatcher;
typedef BasicWatcher<Artist, &Session::metadataUpdated>    ArtistWatcher;
typedef BasicWatcher<Album, &Session::metadataUpdated>     AlbumWatcher;
typedef BasicWatcher<User, &Session::metadataUpdated>      UserWatcher;
typedef BasicWatcher<Search, &Session::objectLoaded>       SearchWatcher;

}
