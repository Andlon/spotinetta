#pragma once

#include "playlist.h"
#include <QObject>

template <class T> class QVector;

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
    void renamed();
    void updateStarted();
    void updateFinished();
    void metadataUpdated();
    void descriptionChanged();
    void imageChanged();
    void subscribersChanged();

    void tracksAdded(const Spotinetta::TrackList &tracks, int position);
    void tracksRemoved(const QVector<int> &positions);
    void tracksMoved(const QVector<int> &positions, int newPosition);

    void trackSeenChanged(int position);
    void trackMessageChanged(int position);
    void trackCreatedChanged(int position);

private:
    void subscribe();
    void unsubscribe();

private slots:
    void onReleased();

private:
    sp_playlist_callbacks   m_callbacks;
    Playlist                m_watched;
};

}
