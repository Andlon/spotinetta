#pragma once

#include "session.h"

#include <QPointer>

namespace Spotinetta {

// We need this class to provide BasicMetadataWatcher with the loaded() signal,
// as Q_OBJECT can not be used in template classes
class LoadedSignalProvider : public QObject {
    Q_OBJECT
public:
    explicit LoadedSignalBase(QObject * parent = 0) : QObject(parent) { }

signals:
    void loaded();
};

template <class WatchedType>
class BasicMetadataWatcher : public LoadedSignalProvider
{
public:
    explicit BasicMetadataWatcher(const Session * session, QObject * parent = 0);

    WatchedType watched() const;
    void watch(const WatchedType &object);

private:
    void checkLoaded();

    WatchedType             m_watched;
    QPointer<const Session> m_session;
};

template <class WatchedType>
inline BasicMetadataWatcher<WatchedType>::BasicMetadataWatcher(const Session * session, QObject * parent = 0)
    :   LoadedSignalBase(parent), m_session(session)
{ }

template <class WatchedType>
inline WatchedType BasicMetadataWatcher<WatchedType>::watched() const
{
    return m_watched;
}

template <class WatchedType>
inline void BasicMetadataWatcher<WatchedType>::watch(const WatchedType &object)
{
    if (object != watched())
    {
        if (m_watched.isValid() && !m_session.isNull())
            disconnect(m_session, &Session::metadataUpdated, this, &BasicMetadataWatcher<WatchedType>::checkLoaded);

        m_watched = object;

        if (m_watched.isValid() && !m_watched.isLoaded() && !m_session.isNull())
        {
            connect(m_session, &Session::metadataUpdated, this, &BasicMetadataWatcher<WatchedType>::checkLoaded);
        }
    }
}

template <class WatchedType>
inline void BasicMetadataWatcher<WatchedType>::checkLoaded()
{
    if (watched().isLoaded() && !m_session.isNull())
    {
        disconnect(m_session, &Session::metadataUpdated, this, &BasicMetadataWatcher<WatchedType>::checkLoaded);
        emit loaded();
    }
}

}
