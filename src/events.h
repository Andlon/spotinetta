#pragma once

#include <QEvent>

#include <Spotinetta/error.h>

namespace Spotinetta {

class Event : public QEvent
{
public:
    enum class Type {
        LoginEvent = QEvent::User + 5000,
        LogoutEvent,
        ConnectionErrorEvent,
        ConnectionStateUpdatedEvent,
        MetadataUpdatedEvent,
        LogEvent,
        NotifyMainThreadEvent,
        StartPlaybackEvent,
        StopPlaybackEvent,
        EndOfTrackEvent,
        StreamingErrorEvent,
        SearchCompleteEvent
    };

    Event(Type type, Error error = Error::Ok, const QByteArray &data = QByteArray(), void * userdata = 0);

    Error       error() const;
    QByteArray  data() const;
    void *      userdata() const;

private:
    QByteArray m_data;
    Error m_error;
    void * m_userdata;
};

inline Event::Event(Type type, Error error, const QByteArray &data, void * userdata)
    :   QEvent(static_cast<QEvent::Type>(type)),m_data(data),  m_error(error), m_userdata(userdata)
{ }

inline Error Event::error() const
{
    return m_error;
}

inline QByteArray Event::data() const
{
    return m_data;
}

inline void * Event::userdata() const
{
    return m_userdata;
}

}
