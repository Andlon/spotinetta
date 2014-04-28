#pragma once

#include "definitions.h"
#include "error.h"
#include "audiooutput.h"
#include "track.h"

#include "search.h"

// See comment in private member of Session to see why this is included
#include "playlistcontainer.h"

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
        initiallyUnloadPlaylists(false),
        audioOutput(nullptr)
    { }

    // libspotify configuration
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

    // Spotinetta configuration
    QSharedPointer<AudioOutputInterface> audioOutput;
};

class Session : public QObject {
    Q_OBJECT

    Q_ENUMS(ConnectionState)
    Q_ENUMS(PlaybackState)
    Q_ENUMS(Error)

    Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
    Q_PROPERTY(Spotinetta::PlaylistContainer playlistContainer READ playlistContainer NOTIFY connectionStateChanged)

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
    ~Session();

    bool            isValid() const;
    Error           error() const;
    sp_session *    handle() const;

    QSharedPointer<AudioOutputInterface> audioOutput() const;

    ConnectionState connectionState() const;
    PlaybackState   playbackState() const;

    PlaylistContainer playlistContainer() const;

    Image createImage(ImageId id) const;
    Image createImageFromLink(const Link &link) const;

    Search search(const QString &query, int trackOffset, int maxTracks,
                         int albumOffset, int maxAlbums,
                         int artistOffset, int maxArtists,
                         int playlistOffset, int maxPlaylists,
                         Search::Type type) const;

    AlbumBrowse browse(const Album & album) const;
    ArtistBrowse browse(const Artist & artist, ArtistBrowseType type = ArtistBrowseType::NoTracks) const;

signals:
    void connectionStateChanged();
    void playbackStateChanged();

    void metadataUpdated();
    void objectLoaded();
    void imageLoaded();

    void loggedIn();
    void loggedOut();
    void endOfTrack();

    void loginFailed(Error error);
    void connectionError(Error error);
    void streamingError(Error error);
    void log(QString message);

    void released();

public slots:
    void login(const QString &username, const QString &password, bool rememberMe = false);
    void logout();
    bool relogin();
    void forgetMe();

    Track load(const Track &track);
    void unload();
    bool play();
    bool pause();
    void seek(int ms);

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

    // While we shouldn't need to store the root container,
    // it is (atleast for now) necessary to workaround a bug that causes libspotify
    // to crash upon logout/session release
    PlaylistContainer           m_rootContainer;

    QTimer *                    m_processTimer;

};

}
