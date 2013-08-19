#include <Spotinetta/playlistcontainerwatcher.h>
#include <Spotinetta/session.h>

#include <QCoreApplication>

namespace Spotinetta {

namespace {

void SP_CALLCONV handleContainerLoaded(sp_playlistcontainer *pc, void *userdata);


struct ContainerEvent : public QEvent {
public:
    enum class Type {
        Loaded = QEvent::User,
        PlaylistAdded = QEvent::User + 1,
        PlaylistMoved = QEvent::User + 2,
        PlaylistRemoved = QEvent::User + 3
    };

    explicit ContainerEvent(Type type) : QEvent(static_cast<QEvent::Type>(type)) { }

    int position;
    int newPosition;
};

}

PlaylistContainerWatcher::PlaylistContainerWatcher(const Session *session, QObject *parent)
    :   QObject(parent)
{
    memset(&m_callbacks, 0, sizeof(sp_playlistcontainer_callbacks));
    m_callbacks.container_loaded = &handleContainerLoaded;

    // When the session's being released, we need to make sure any registered callbacks
    // are unregistered. We therefore connect to the Session destroyed() signal and
    // simply watch an invalid PlaylistContainer
    connect(session, &Session::destroyed, [this] {
        watch(PlaylistContainer());
    });
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

void PlaylistContainerWatcher::customEvent(QEvent * event)
{
    Q_ASSERT(event->type() >= QEvent::User && event->type() <= QEvent::User + 3);

    switch (static_cast<ContainerEvent::Type>(event->type()))
    {
    case (ContainerEvent::Type::Loaded):
        // Container loaded
        emit loaded();

    default:
        break;
    }
}

namespace {

void SP_CALLCONV handleContainerLoaded(sp_playlistcontainer *, void *userdata)
{
    PlaylistContainerWatcher * watcher = static_cast<PlaylistContainerWatcher *>(userdata);
    ContainerEvent event(ContainerEvent::Type::Loaded);
    QCoreApplication::sendEvent(watcher, &event);
}

}


}
