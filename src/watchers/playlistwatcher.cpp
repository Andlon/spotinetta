#include <Spotinetta/playlistwatcher.h>
#include <Spotinetta/session.h>
#include <Spotinetta/track.h>

#include <QCoreApplication>
#include <QMetaMethod>

namespace Spotinetta {

namespace {
void SP_CALLCONV handlePlaylistMetadataUpdated(sp_playlist * pl, void * userdata);
void SP_CALLCONV handlePlaylistStateChanged(sp_playlist * pl, void *userdata);
void SP_CALLCONV handlePlaylistUpdateInProgress(sp_playlist *pl, bool done, void *userdata);
void SP_CALLCONV handleDescriptionChanged(sp_playlist *pl, const char *desc, void *userdata);
void SP_CALLCONV handlePlaylistRenamed(sp_playlist *pl, void *userdata);
void SP_CALLCONV handleImageChanged(sp_playlist *pl, const byte *image, void *userdata);
void SP_CALLCONV handleTracksAdded(sp_playlist *pl, sp_track *const *tracks, int num_tracks, int position, void *userdata);
void SP_CALLCONV handleTracksRemoved(sp_playlist *pl, const int *tracks, int num_tracks, void *userdata);
void SP_CALLCONV handleTracksMoved(sp_playlist *pl, const int *tracks, int num_tracks, int new_position, void *userdata);
void SP_CALLCONV handleSubscriberschanged(sp_playlist *pl, void *userdata);
void SP_CALLCONV handleTrackSeenChanged(sp_playlist *pl, int position, bool seen, void *userdata);
void SP_CALLCONV handleTrackMessageChanged(sp_playlist *pl, int position, const char *message, void *userdata);
void SP_CALLCONV handleTrackCreatedChanged(sp_playlist *pl, int position, sp_user *user, int when, void *userdata);

}

PlaylistWatcher::PlaylistWatcher(const Session *session, QObject *parent)
    :   QObject(parent)
{
    memset(&m_callbacks, 0, sizeof(sp_playlist_callbacks));
    m_callbacks.description_changed = &handleDescriptionChanged;
    m_callbacks.image_changed = &handleImageChanged;
    m_callbacks.playlist_metadata_updated = &handlePlaylistMetadataUpdated;
    m_callbacks.playlist_renamed = &handlePlaylistRenamed;
    m_callbacks.playlist_state_changed = &handlePlaylistStateChanged;
    m_callbacks.playlist_update_in_progress = &handlePlaylistUpdateInProgress;
    m_callbacks.subscribers_changed = &handleSubscriberschanged;
    m_callbacks.tracks_added = &handleTracksAdded;
    m_callbacks.tracks_moved = &handleTracksMoved;
    m_callbacks.tracks_removed = &handleTracksRemoved;
    m_callbacks.track_created_changed = &handleTrackCreatedChanged;
    m_callbacks.track_message_changed = &handleTrackMessageChanged;
    m_callbacks.track_seen_changed = &handleTrackSeenChanged;

    // Clean up callbacks by watching an invalid object before session release
    connect(session, &Session::destroyed, [this] {
        watch(Playlist());
    });
}

PlaylistWatcher::~PlaylistWatcher()
{
    unsubscribe();
}

Playlist PlaylistWatcher::watched() const
{
    return m_watched;
}

void PlaylistWatcher::watch(const Playlist &playlist)
{
    if (playlist != watched())
    {
        unsubscribe();
        m_watched = playlist;
        subscribe();
    }
}

void PlaylistWatcher::subscribe()
{
    Playlist current = watched();
    if (current.isValid())
    {
        sp_playlist_add_callbacks(current.handle(), &m_callbacks, static_cast<void *>(this));
    }
}

void PlaylistWatcher::unsubscribe()
{
    Playlist current = watched();
    if (current.isValid())
    {
        sp_playlist_remove_callbacks(current.handle(), &m_callbacks, static_cast<void *>(this));
    }
}

namespace {
void SP_CALLCONV handlePlaylistStateChanged(sp_playlist *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::stateChanged);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handlePlaylistMetadataUpdated(sp_playlist *, void * userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::metadataUpdated);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handlePlaylistUpdateInProgress(sp_playlist *, bool done, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(done ? &PlaylistWatcher::updateFinished : &PlaylistWatcher::updateStarted);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handleDescriptionChanged(sp_playlist *, const char *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::descriptionChanged);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handlePlaylistRenamed(sp_playlist *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::renamed);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handleImageChanged(sp_playlist *, const byte *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::imageChanged);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handleSubscriberschanged(sp_playlist *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::subscribersChanged);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handleTracksAdded(sp_playlist *, sp_track *const *tracks, int num_tracks, int position, void *userdata)
{
    TrackList list;
    list.reserve(num_tracks);
    for (int i = 0; i < num_tracks; ++i)
        list.append(Track(tracks[i]));

    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::tracksAdded);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(TrackList, list), Q_ARG(int, position));
}

void SP_CALLCONV handleTracksRemoved(sp_playlist *, const int *tracks, int num_tracks, void *userdata)
{
    QVector<int> positions;
    positions.reserve(num_tracks);

    for (int i = 0; i < num_tracks; ++i)
        positions.append(tracks[i]);

    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::tracksRemoved);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(QVector<int>, positions));
}

void SP_CALLCONV handleTracksMoved(sp_playlist *, const int *tracks, int num_tracks, int new_position, void *userdata)
{
    QVector<int> positions;
    positions.reserve(num_tracks);

    for (int i = 0; i < num_tracks; ++i)
        positions.append(tracks[i]);

    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::tracksMoved);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(QVector<int>, positions), Q_ARG(int, new_position));
}

void SP_CALLCONV handleTrackSeenChanged(sp_playlist *, int position, bool, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::trackSeenChanged);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

void SP_CALLCONV handleTrackMessageChanged(sp_playlist *, int position, const char *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::trackMessageChanged);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

void SP_CALLCONV handleTrackCreatedChanged(sp_playlist *, int position, sp_user *, int, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistWatcher::trackCreatedChanged);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

}

}
