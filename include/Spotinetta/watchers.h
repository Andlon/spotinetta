#pragma once

#include "basicmetadatawatcher.h"
#include "playlistcontainerwatcher.h"
#include "playlistwatcher.h"

#include "track.h"
#include "artist.h"
#include "album.h"

namespace Spotinetta {

typedef BasicMetadataWatcher<Track>     TrackWatcher;
typedef BasicMetadataWatcher<Artist>    ArtistWatcher;
typedef BasicMetadataWatcher<Album>     AlbumWatcher;

}
