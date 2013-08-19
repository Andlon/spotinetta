#include <Spotinetta/playlistwatcher.h>
#include <Spotinetta/session.h>

#include <QCoreApplication>

namespace Spotinetta {

namespace {
void SP_CALLCONV handlePlaylistStateChanged(sp_playlist *pl, void *userdata);

struct PlaylistEvent : public QEvent {
public:
    enum class Type {
        StateChanged = QEvent::User
    };

    explicit PlaylistEvent(Type type) : QEvent(static_cast<QEvent::Type>(type)) { }


};
}

PlaylistWatcher::PlaylistWatcher(const Session *session, QObject *parent)
    :   QObject(parent)
{
    memset(&m_callbacks, 0, sizeof(sp_playlist_callbacks));
    m_callbacks.playlist_state_changed = &handlePlaylistStateChanged;

    // Clear up callbacks by watching an invalid object before session release
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

void PlaylistWatcher::customEvent(QEvent * event)
{
    // Add an assertion here

    switch (static_cast<PlaylistEvent::Type>(event->type()))
    {
    case (PlaylistEvent::Type::StateChanged):
        emit stateChanged();

    default:
        break;
    }

}

namespace {
void SP_CALLCONV handlePlaylistStateChanged(sp_playlist *, void *userdata)
{
    PlaylistWatcher * watcher = static_cast<PlaylistWatcher *>(userdata);
    PlaylistEvent event(PlaylistEvent::Type::StateChanged);
    QCoreApplication::sendEvent(watcher, &event);
}

}

}
