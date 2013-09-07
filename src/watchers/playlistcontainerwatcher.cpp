#include <Spotinetta/playlistcontainerwatcher.h>
#include <Spotinetta/playlist.h>
#include <Spotinetta/session.h>

#include <QMetaMethod>

namespace Spotinetta {

namespace {
void SP_CALLCONV handleContainerLoaded(sp_playlistcontainer *pc, void *userdata);
void SP_CALLCONV handlePlaylistAdded(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata);
void SP_CALLCONV handlePlaylistRemoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata);
void SP_CALLCONV handlePlaylistMoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata);
}

PlaylistContainerWatcher::PlaylistContainerWatcher(const Session *session, QObject *parent)
    :   QObject(parent)
{
    memset(&m_callbacks, 0, sizeof(sp_playlistcontainer_callbacks));
    m_callbacks.container_loaded = &handleContainerLoaded;

    // When the session's being released, we need to make sure any registered callbacks
    // are unregistered. We therefore connect to the Session released() signal and
    // simply watch an invalid PlaylistContainer
    connect(session, &Session::released, this, &PlaylistContainerWatcher::onReleased);
}

void PlaylistContainerWatcher::onReleased()
{
    watch(PlaylistContainer());
}

PlaylistContainerWatcher::~PlaylistContainerWatcher()
{
    unsubscribe();
}

PlaylistContainer PlaylistContainerWatcher::watched() const
{
    return m_watched;
}

void PlaylistContainerWatcher::watch(const PlaylistContainer &object)
{
    qDebug() << "Handle: " << m_watched.handle();
    if (object != watched())
    {
        unsubscribe();
        m_watched = object;
        subscribe();
    }
}

void PlaylistContainerWatcher::subscribe()
{
    PlaylistContainer current = watched();
    if (current.isValid())
    {
        sp_playlistcontainer_add_callbacks(current.handle(), &m_callbacks, static_cast<void *>(this));
    }
}

void PlaylistContainerWatcher::unsubscribe()
{
    PlaylistContainer current = watched();
    if (current.isValid())
    {
        sp_playlistcontainer_remove_callbacks(current.handle(), &m_callbacks, static_cast<void *>(this));
    }
}

namespace {

void SP_CALLCONV handleContainerLoaded(sp_playlistcontainer *, void *userdata)
{
    PlaylistContainerWatcher * watcher = static_cast<PlaylistContainerWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistContainerWatcher::loaded);
    signal.invoke(watcher, Qt::DirectConnection);
}

void SP_CALLCONV handlePlaylistAdded(sp_playlistcontainer *, sp_playlist *, int position, void *userdata)
{
    PlaylistContainerWatcher * watcher = static_cast<PlaylistContainerWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistContainerWatcher::playlistAdded);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

void SP_CALLCONV handlePlaylistRemoved(sp_playlistcontainer *, sp_playlist *, int position, void *userdata)
{
    PlaylistContainerWatcher * watcher = static_cast<PlaylistContainerWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistContainerWatcher::playlistRemoved);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

void SP_CALLCONV handlePlaylistMoved(sp_playlistcontainer *, sp_playlist *, int position, void *userdata)
{
    PlaylistContainerWatcher * watcher = static_cast<PlaylistContainerWatcher *>(userdata);
    QMetaMethod signal = QMetaMethod::fromSignal(&PlaylistContainerWatcher::playlistMoved);
    signal.invoke(watcher, Qt::DirectConnection, Q_ARG(int, position));
}

}


}
