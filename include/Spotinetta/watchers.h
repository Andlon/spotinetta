#pragma once

#include "basicwatcher.h"
#include "playlistcontainerwatcher.h"
#include "playlistwatcher.h"
#include "imagewatcher.h"

#include "objects.h"

namespace Spotinetta {

typedef BasicWatcher<Track, &Session::metadataUpdated>     TrackWatcher;
typedef BasicWatcher<Artist, &Session::metadataUpdated>    ArtistWatcher;
typedef BasicWatcher<Album, &Session::metadataUpdated>     AlbumWatcher;
typedef BasicWatcher<User, &Session::metadataUpdated>      UserWatcher;
typedef BasicWatcher<Search, &Session::objectLoaded>       SearchWatcher;
typedef BasicWatcher<ArtistBrowse, &Session::objectLoaded> ArtistBrowseWatcher;
typedef BasicWatcher<AlbumBrowse, &Session::objectLoaded>  AlbumBrowseWatcher;

}
