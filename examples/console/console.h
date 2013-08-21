#pragma once

#include "consoleinput.h"

#include <QObject>
#include <QPointer>
#include <QTextStream>
#include <QMutex>

// Make sure to include this before session, to get the extra
// QAudioFormat conversions
#include <QtMultimedia/QAudioOutput>

#include <Spotinetta/detail/ringbuffer.h>


#include <Spotinetta/session.h>

class QAudioOutput;
class QIODevice;

class AudioOutput : public QObject, public Spotinetta::AudioOutputInterface
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject * parent = 0);
    ~AudioOutput();
    int deliver(const Spotinetta::AudioFrameCollection &collection);
    void reset();

private slots:
    void push();

private:
    void setupOutput(const QAudioFormat &format);

    QPointer<QAudioOutput> m_output;
    QPointer<QIODevice>    m_device;
    QAudioFormat           m_format;
    QMutex                 m_formatLock;

    Spotinetta::detail::RingBuffer<char, 1024> m_buffer;
};

class Console : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Idle,
        Waiting,
        Quitting,
        LoginUsername,
        LoginPassword
    };

    explicit Console(Spotinetta::Session * session, QObject *parent = 0);

    void start();
    
private slots:
    void onLoggedIn();
    void onLoggedOut();
    void onLoginFailed(Spotinetta::Error);

    void onConsoleInput(const QString &line);
    void onLogMessage(const QString &message);
    
private:
    void login();
    void logout();
    void block();
    void unblock();
    void quit();

    void processState();
    void processIdleInput(const QStringList &words);

    QPointer<Spotinetta::Session> m_session;
    State   m_state;

    ConsoleInput * in;
    QTextStream out;

    QString m_loginUsername;
    QString m_loginPassword;

};
