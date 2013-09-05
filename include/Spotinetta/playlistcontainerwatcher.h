#pragma once

#include "playlistcontainer.h"
#include <QObject>

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
    void playlistAdded(int position);
    void playlistRemoved(int position);
    void playlistMoved(int oldPosition, int newPosition);

private:
    void subscribe();
    void unsubscribe();

private slots:
    void onReleased();

private:
    sp_playlistcontainer_callbacks  m_callbacks;
    PlaylistContainer               m_watched;
};

}
