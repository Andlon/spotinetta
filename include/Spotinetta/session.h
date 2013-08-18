#pragma once

#include "definitions.h"
#include "error.h"

#include <QObject>
#include <QSharedPointer>
#include <QVector>

#include <cstdint>

// Forward declarations
class QTimer;

namespace Spotinetta {

class ApplicationKey {
public:
    ApplicationKey() { }
    ApplicationKey(const uint8_t * key, size_t size);

    const uint8_t * data() const;
    size_t size() const;

private:
    QVector<uint8_t> m_data;
};

struct SessionConfig {
    SessionConfig() :
        compressPlaylists(false),
        dontSaveMetadataForPlaylists(false),
        initiallyUnloadPlaylists(false)
    { }

    ApplicationKey applicationKey;
    QString cacheLocation;
    QString settingsLocation;
    QString userAgent;
    QString deviceId;
    QString proxy;
    QString proxyUsername;
    QString proxyPassword;
    QString caCertsFilename;
    QString tracefile;

    bool compressPlaylists;
    bool dontSaveMetadataForPlaylists;
    bool initiallyUnloadPlaylists;
};

class Session : public QObject {
    Q_OBJECT

    Q_ENUMS(ConnectionState)
    Q_ENUMS(PlaybackState)
    Q_ENUMS(Error)

    Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)

public:
    typedef ::Spotinetta::Error Error;

    enum class ConnectionState {
        LoggedOut = SP_CONNECTION_STATE_LOGGED_OUT,
        LoggedIn = SP_CONNECTION_STATE_LOGGED_IN,
        Disconnected = SP_CONNECTION_STATE_DISCONNECTED,
        Undefined = SP_CONNECTION_STATE_UNDEFINED,
        Offline = SP_CONNECTION_STATE_OFFLINE
    };

    enum class PlaybackState {
        Playing,
        Paused
    };

    explicit Session(const SessionConfig &config, QObject * parent = 0);

    bool            isValid() const;
    Error           error() const;
    sp_session *    handle() const;

    ConnectionState connectionState() const;
    PlaybackState   playbackState() const;

    Image createImage(const byte * id) const;

signals:
    void connectionStateChanged();
    void playbackStateChanged();

    void metadataUpdated();

    void loggedIn();
    void loggedOut();
    void endOfTrack();

    void loginFailed(Error error);
    void connectionError(Error error);
    void streamingError(Error error);
    void log(QString message);

public slots:
    void login(const QString &username, const QString &password, bool rememberMe = false);
    void logout();

    bool load(const Track &track);
    bool play();
    bool pause();

protected:
    void customEvent(QEvent *);

private slots:
    void processEvents();

private:
    QSharedPointer<sp_session>  m_handle;
    ApplicationKey              m_key;
    SessionConfig               m_config;
    Error                       m_error;
    PlaybackState               m_playbackState;

    QTimer *                    m_processTimer;

};

}
