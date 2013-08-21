#pragma once

#include "consoleinput.h"

#include <QObject>
#include <QPointer>
#include <QTextStream>
#include <QMutex>

// Make sure to include this before session, to get the extra
// QAudioFormat conversions
#include <QtMultimedia/QAudioOutput>


#include <Spotinetta/session.h>

class QAudioOutput;
class QIODevice;

class AudioOutput : public Spotinetta::AudioOutputInterface
{
public:
    ~AudioOutput();
    int deliver(const Spotinetta::AudioFrameCollection &collection);
    void reset();

private:
    QPointer<QAudioOutput> m_output;
    QPointer<QIODevice>    m_device;
    QMutex                 m_lock;
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
