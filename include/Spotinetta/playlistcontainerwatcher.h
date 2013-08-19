#pragma once

#include "playlistcontainer.h"
#include <QObject>

class QEvent;

namespace Spotinetta {

class PlaylistContainerWatcher : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistContainerWatcher(const Session * session, QObject * parent = 0);
    ~PlaylistContainerWatcher();

    PlaylistContainer watched() const;
    void watch(const PlaylistContainer &object);

signals:
    void loaded();

protected:
    void customEvent(QEvent *);

private:
    void subscribe();
    void unsubscribe();

private:
    sp_playlistcontainer_callbacks  m_callbacks;
    PlaylistContainer               m_watched;
};

}
