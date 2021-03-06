#pragma once

#include "session.h"

#include <QPointer>

namespace Spotinetta {

// We need this class to provide BasicMetadataWatcher with the loaded() signal,
// as Q_OBJECT can not be used in template classes
class LoadedSignalProvider : public QObject {
    Q_OBJECT
public:
    explicit LoadedSignalProvider(QObject * parent = 0) : QObject(parent) { }

signals:
    void loaded();
};

template <class WatchedType, void (Session::*LoadedSignal)()>
class BasicWatcher : public LoadedSignalProvider
{
public:
    explicit BasicWatcher(const Session * session, QObject * parent = 0);

    WatchedType watched() const;
    void watch(const WatchedType &object);

private:
    void checkLoaded();

    WatchedType             m_watched;
    QPointer<const Session> m_session;
};

template <class WatchedType, void (Session::*LoadedSignal)()>
inline BasicWatcher<WatchedType, LoadedSignal>::BasicWatcher(const Session * session, QObject * parent)
    :   LoadedSignalProvider(parent), m_session(session)
{
    Q_ASSERT_X(!m_session.isNull(), "BasicWatcher::BasicWatcher", "Null session");
    Q_ASSERT_X(m_session->thread() == this->thread(), "BasicWatcher::BasicWatcher", "Watcher and session do not live in the same thread");
}

template <class WatchedType, void (Session::*LoadedSignal)()>
inline WatchedType BasicWatcher<WatchedType, LoadedSignal>::watched() const
{
    return m_watched;
}

template <class WatchedType, void (Session::*LoadedSignal)()>
inline void BasicWatcher<WatchedType, LoadedSignal>::watch(const WatchedType &object)
{
    if (object != watched())
    {
        if (m_watched.isValid() && !m_session.isNull())
            disconnect(m_session.data(), LoadedSignal, this, &BasicWatcher<WatchedType, LoadedSignal>::checkLoaded);

        m_watched = object;

        if (m_watched.isValid() && !m_watched.isLoaded() && !m_session.isNull())
        {
            connect(m_session.data(), LoadedSignal, this, &BasicWatcher<WatchedType, LoadedSignal>::checkLoaded);
        }
    }
}

template <class WatchedType, void (Session::*LoadedSignal)()>
inline void BasicWatcher<WatchedType, LoadedSignal>::checkLoaded()
{
    if (watched().isLoaded() && !m_session.isNull())
    {
        disconnect(m_session.data(), LoadedSignal, this, &BasicWatcher<WatchedType, LoadedSignal>::checkLoaded);
        emit loaded();
    }
}

}
