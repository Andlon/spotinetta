#pragma once

#include "consoleinput.h"

#include <QObject>
#include <QPointer>
#include <QTextStream>
#include <Spotinetta/session.h>

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
