#ifndef SPOTINETTACONSOLE_H
#define SPOTINETTACONSOLE_H

#include <QObject>
#include <QPointer>
#include <QTextStream>
#include <Spotinetta/session.h>

class SpotinettaConsole : public QObject
{
    Q_OBJECT
public:
    explicit SpotinettaConsole(Spotinetta::Session * session, QObject *parent = 0);

    void start();
    
signals:
    
private slots:
    void onLoggedIn();
    void onLoginFailed(Spotinetta::Error);
    
private:
    QPointer<Spotinetta::Session> m_session;

    QTextStream in;
    QTextStream out;
};

#endif // SPOTINETTACONSOLE_H
