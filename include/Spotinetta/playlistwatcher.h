#pragma once

#include "playlist.h"
#include <QObject>

class QEvent;

namespace Spotinetta {

class PlaylistWatcher : public QObject {
    Q_OBJECT
public:
    explicit PlaylistWatcher(const Session * session, QObject * parent = 0);
    ~PlaylistWatcher();

    Playlist watched() const;
    void watch(const Playlist &playlist);

signals:
    void stateChanged();

protected:
    void customEvent(QEvent *);

private:
    void subscribe();
    void unsubscribe();

private:
    sp_playlist_callbacks   m_callbacks;
    Playlist                m_watched;
};

}
