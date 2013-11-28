#include <Spotinetta/session.h>
#include <Spotinetta/objects.h>
#include "events.h"

#include <QCoreApplication>
#include <QtAlgorithms>
#include <QTimer>
#include <QDebug>

namespace Spotinetta {

void registerTypes()
{
    qRegisterMetaType<Spotinetta::Track>();
    qRegisterMetaType<Spotinetta::Album>();
    qRegisterMetaType<Spotinetta::Artist>();
    qRegisterMetaType<Spotinetta::Playlist>();
    qRegisterMetaType<Spotinetta::PlaylistContainer>();
    qRegisterMetaType<Spotinetta::Search>();
    qRegisterMetaType<Spotinetta::ArtistBrowse>();
    qRegisterMetaType<Spotinetta::AlbumBrowse>();
    qRegisterMetaType<Spotinetta::Image>();
    qRegisterMetaType<Spotinetta::Link>();
    qRegisterMetaType<Spotinetta::User>();
    qRegisterMetaType<Spotinetta::TrackList>();
    qRegisterMetaType<Spotinetta::AlbumList>();
    qRegisterMetaType<Spotinetta::SearchList>();
    qRegisterMetaType<Spotinetta::ImageList>();
}

namespace {

// Declare callbacks
void SP_CALLCONV handleLoggedIn(sp_session *, sp_error);
void SP_CALLCONV handleLoggedOut(sp_session *);
void SP_CALLCONV handleConnectionError(sp_session *, sp_error);
void SP_CALLCONV handleConnectionStateUpdated(sp_session *);
void SP_CALLCONV handleNotifyMainThread(sp_session *);
void SP_CALLCONV handleLogMessage(sp_session *, const char *);
void SP_CALLCONV handleMetadataUpdated(sp_session *);
int  SP_CALLCONV handleMusicDelivery(sp_session *, const sp_audioformat *, const void *, int);
void SP_CALLCONV handleStreamingError(sp_session *, sp_error);
void SP_CALLCONV handleEndOfTrack(sp_session *);
//void SP_CALLCONV handleGetAudioBufferStats(sp_session *, sp_audio_buffer_stats *) { }
//void SP_CALLCONV handleStartPlayback(sp_session *) { }
//void SP_CALLCONV handleStopPlayback(sp_session *) { }
void SP_CALLCONV handleSearchComplete(sp_search *, void *);

}

ApplicationKey::ApplicationKey(const uint8_t *key, size_t size)
    :   m_data(size)
{
    qCopy(key, key + size, m_data.begin());
}

const uint8_t * ApplicationKey::data() const
{
    return m_data.constData();
}

size_t ApplicationKey::size() const
{
    return static_cast<size_t>(m_data.size());
}

Session::Session(const SessionConfig &config, QObject *parent)
    :   QObject(parent), m_playbackState(PlaybackState::Paused), m_processTimer(new QTimer(this))
{
    m_config = config;

    sp_session_config       spconfig;
    sp_session_callbacks    callbacks;

    sp_session * session;

    memset(&spconfig, 0, sizeof(sp_session_config));
    memset(&callbacks, 0, sizeof(sp_session_callbacks));

    QByteArray userAgent = m_config.userAgent.toUtf8();
    QByteArray cacheLocation = m_config.cacheLocation.toUtf8();
    QByteArray settingsLocation = m_config.settingsLocation.toUtf8();
    QByteArray deviceId = m_config.deviceId.toUtf8();
    QByteArray proxy = m_config.proxy.toUtf8();
    QByteArray proxyUsername = m_config.proxyUsername.toUtf8();
    QByteArray proxyPassword = m_config.proxyPassword.toUtf8();
    QByteArray tracefile = m_config.tracefile.toUtf8();

    // For header/lib compatibility check
    spconfig.api_version = SPOTIFY_API_VERSION;

    //    // Use the userdata field to store the address of this Session object,
    //    // so that we may forward events from the callbacks
    spconfig.userdata = static_cast<void *>(this);

    spconfig.application_key = m_config.applicationKey.data();
    spconfig.application_key_size = m_config.applicationKey.size();

    spconfig.user_agent = userAgent.isEmpty() ? 0 : userAgent.constData();
    spconfig.cache_location = cacheLocation.isEmpty() ? "" : cacheLocation.constData();
    spconfig.settings_location = settingsLocation.isEmpty() ? "" : settingsLocation.constData();
    spconfig.device_id = deviceId.isEmpty() ? 0 : deviceId.constData();
    spconfig.proxy = proxy.isEmpty() ? 0 : proxy.constData();
    spconfig.proxy_username = proxyUsername.isEmpty() ? 0 : proxyUsername.constData();
    spconfig.proxy_password = proxyPassword.isEmpty() ? 0 : proxyPassword.constData();
    spconfig.tracefile = tracefile.isEmpty() ? 0 : tracefile.constData();

    spconfig.compress_playlists = m_config.compressPlaylists;
    spconfig.dont_save_metadata_for_playlists = m_config.dontSaveMetadataForPlaylists;
    spconfig.initially_unload_playlists = m_config.initiallyUnloadPlaylists;

    callbacks.logged_in = &handleLoggedIn;
    callbacks.logged_out = &handleLoggedOut;
    callbacks.connection_error = &handleConnectionError;
    callbacks.connectionstate_updated = &handleConnectionStateUpdated;
    callbacks.log_message = &handleLogMessage;
    callbacks.notify_main_thread = &handleNotifyMainThread;
    callbacks.music_delivery = &handleMusicDelivery;
    callbacks.metadata_updated = &handleMetadataUpdated;
    callbacks.streaming_error = &handleStreamingError;
    callbacks.end_of_track = &handleEndOfTrack;
    //    callbacks.get_audio_buffer_stats = &handleGetAudioBufferStats;
    //    callbacks.start_playback = &handleStartPlayback;
    //    callbacks.stop_playback = &handleStopPlayback;

    spconfig.callbacks = &callbacks;

    m_error = static_cast<Error>(sp_session_create(&spconfig, &session));

    if (m_error == Error::Ok)
    {
        connect(this, &Session::endOfTrack, this, &Session::playbackStateChanged);

        m_handle.reset(session, &sp_session_release);
        processEvents();
    }
}

Session::~Session()
{
    // libspotify seems to crash if we don't unload the current track
    // before exiting (unless we've properly logged out)
    sp_session_player_unload(handle());

    // When we reach the end of this scope, the session is released,
    // thus we emit this signal before, so that watchers and others may react on it
    emit released();

    // Reset playlistcontainer so that the deref function is not called
    // after session destruction
    m_rootContainer = PlaylistContainer();


}

void Session::processEvents()
{
    int next = 0;
    do {
        sp_session_process_events(handle(), &next);
    } while (next == 0);

    m_processTimer->start(next);
}

bool Session::isValid() const
{
    return m_handle.data() != 0;
}

Error Session::error() const
{
    return m_error;
}

sp_session * Session::handle() const
{
    return m_handle.data();
}

QSharedPointer<AudioOutputInterface> Session::audioOutput() const
{
    return m_config.audioOutput;
}

Session::ConnectionState Session::connectionState() const
{
    return isValid() ? static_cast<ConnectionState>(sp_session_connectionstate(handle())) : ConnectionState::Undefined;
}

Session::PlaybackState Session::playbackState() const
{
    return m_playbackState;
}

PlaylistContainer Session::playlistContainer() const
{
    return m_rootContainer;
}

Image Session::createImage(ImageId id) const
{
    if (isValid() && id != 0)
    {
        // Make sure to _not_ increment reference count upon creation,
        // as sp_image_create pre-increments
        return Image(sp_image_create(handle(), id), false);
    }

    return Image();
}

Image Session::createImageFromLink(const Link &link) const
{
    if (isValid() && link.isValid())
    {
        // Also here, make sure not to increment reference count
        return Image(sp_image_create_from_link(handle(), link.handle()), false);
    }

    return Image();
}

Search Session::createSearch(const QString &query, int trackOffset, int maxTracks, int albumOffset,
                             int maxAlbums, int artistOffset, int maxArtists, int playlistOffset,
                             int maxPlaylists, Search::Type type) const
{
    if (isValid())
    {
        QByteArray queryData = query.toUtf8();
        // Use const_cast here (ugh!) because creating a search object
        // does not actually modify the Session
        return Search(sp_search_create(handle(), queryData.constData(), trackOffset, maxTracks,
                                       albumOffset, maxAlbums, artistOffset, maxArtists,
                                       playlistOffset, maxPlaylists, static_cast<sp_search_type>(type),
                                       &handleSearchComplete,
                                       static_cast<void *>(const_cast<Session *>(this))),
                                       false);
    }

    return Search();
}

void Session::login(const QString &username, const QString &password, bool rememberMe)
{
    if (isValid())
        sp_session_login(handle(), username.toUtf8(), password.toUtf8(), rememberMe, 0);
}

void Session::logout()
{
    if (isValid())
        sp_session_logout(handle());
}

bool Session::relogin()
{
    Error err = static_cast<Error>(sp_session_relogin(handle()));
    return err == Error::Ok;
}

bool Session::play()
{
    if (isValid())
    {
        m_error = static_cast<Error>(sp_session_player_play(handle(), true));

        if (m_error == Error::Ok)
        {
            m_playbackState = PlaybackState::Playing;
            emit playbackStateChanged();
            return true;
        }
    }

    return false;
}

/**
 * @brief Session::load Loads the given track for playback.
 * @param track The track to load.
 * @return The track that was actually loaded. The returned track is invalid if loading failed.
 */

Track Session::load(const Track &track)
{
    if (isValid())
    {
        m_error = static_cast<Error>(sp_session_player_load(handle(), track.handle()));

        if (m_error == Error::Ok)
            return track.playableTrack(this);
    }

    return Track();
}

void Session::unload()
{
    if (isValid())
    {
        sp_session_player_unload(handle());
    }
}

bool Session::pause()
{
    if (isValid())
    {
        m_error = static_cast<Error>(sp_session_player_play(handle(), false));

        if (m_error == Error::Ok)
        {
            m_playbackState = PlaybackState::Paused;
            emit playbackStateChanged();
            return true;
        }
    }

    return false;
}

void Session::seek(int ms)
{
    if (isValid())
    {
        sp_session_player_seek(handle(), ms);
    }
}

void Session::customEvent(QEvent * e)
{
    // Only Spotinetta::Event events should be sent to Session
    Event::Type t = static_cast<Spotinetta::Event::Type>(e->type());
    Event * event = static_cast<Event *>(e);

    switch (t)
    {
    case (Event::Type::LoginEvent):
        if (event->error() == Error::Ok)
        {
            m_rootContainer = PlaylistContainer(sp_session_playlistcontainer(handle()));
            emit loggedIn();
        }
        else
        {
            emit loginFailed(event->error());
        }
        break;
    case (Event::Type::LogoutEvent):
        m_rootContainer = PlaylistContainer();
        emit loggedOut();
        break;
    case (Event::Type::ConnectionStateUpdatedEvent):
        emit connectionStateChanged();
        break;
    case (Event::Type::MetadataUpdatedEvent):
        emit metadataUpdated();
        break;
    case (Event::Type::EndOfTrackEvent):
        m_playbackState = PlaybackState::Paused;
        // playbackStateChanged is connected to endOfTrack
        emit endOfTrack();
        break;
    case (Event::Type::ConnectionErrorEvent):
        emit connectionError(event->error());
        break;
    case (Event::Type::NotifyMainThreadEvent):
        processEvents();
        break;
    case (Event::Type::LogEvent):
        emit log(QString::fromUtf8(event->data()));
        break;
    case (Event::Type::StreamingErrorEvent):
        emit streamingError(event->error());
        break;
    case (Event::Type::SearchCompleteEvent):
        emit objectLoaded();
        break;

    default:
        qWarning() << "Spotinetta::Session received unhandled event.";
        break;
    }
}

namespace {
void SP_CALLCONV handleLoggedIn(sp_session * s, sp_error e) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::LoginEvent, static_cast<Error>(e)));
}

void SP_CALLCONV handleLoggedOut(sp_session * s) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::LogoutEvent));
}

void SP_CALLCONV handleConnectionError(sp_session * s, sp_error e) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::ConnectionErrorEvent, static_cast<Error>(e)));
}

void SP_CALLCONV handleConnectionStateUpdated(sp_session * s) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::ConnectionStateUpdatedEvent));
}

void SP_CALLCONV handleNotifyMainThread(sp_session * s) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::NotifyMainThreadEvent));
}

void SP_CALLCONV handleLogMessage(sp_session * s, const char * message) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::LogEvent, Error::Ok, QByteArray(message).trimmed()));
}

void SP_CALLCONV handleMetadataUpdated(sp_session * s) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::MetadataUpdatedEvent));
}

int  SP_CALLCONV handleMusicDelivery(sp_session * s, const sp_audioformat * f, const void * frames, int frameCount) {
    // Retrieve output, if it exists
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QSharedPointer<AudioOutputInterface> output = session->audioOutput();

    if (f->sample_type != SP_SAMPLETYPE_INT16_NATIVE_ENDIAN)
    {
        // This will spam... a lot.
        qWarning() << "Unsupported sampletype!";
        return 0;
    }

    int consumed = 0;
    if (output != nullptr)
    {
        if (frameCount > 0)
        {
            AudioFormat format(f->sample_rate, f->channels);
            AudioFrameCollection collection(static_cast<const char *>(frames),
                                            format.bytesPerFrame() * frameCount,
                                            format);
            consumed = output->deliver(collection);
        }
        else
        {
            // frameCount == 0, reset buffers
            output->reset();
        }
    }

    return consumed;
}

void SP_CALLCONV handleStreamingError(sp_session * s, sp_error e) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::StreamingErrorEvent, static_cast<Error>(e)));
}

void SP_CALLCONV handleEndOfTrack(sp_session * s) {
    Session * session = static_cast<Session *>(sp_session_userdata(s));
    QCoreApplication::postEvent(session, new Event(Event::Type::EndOfTrackEvent));
}

void SP_CALLCONV handleSearchComplete(sp_search * search, void * userdata)
{
    Session * session = static_cast<Session *>(userdata);
    QCoreApplication::postEvent(session, new Event(Event::Type::SearchCompleteEvent, Error::Ok, QByteArray(), static_cast<void *>(search)));
}

}


}
