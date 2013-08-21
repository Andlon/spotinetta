#include <QCoreApplication>
#include <QDebug>

#include <Spotinetta/session.h>

#include "console.h"
#include "../../appkey.c"

int main(int argc, char *argv[])
{
    using namespace Spotinetta;

    QCoreApplication a(argc, argv);

    AudioOutput output;

    SessionConfig config;
    config.applicationKey = ApplicationKey(g_appkey, g_appkey_size);
    config.userAgent = "Spotinetta Console";
    config.audioOutput = &output;

    qDebug() << "Creating session...";
    Session session(config);
    qDebug() << "Created session.";

    Console console(&session);

    if (session.isValid())
    {
        console.start();
    }
    else
    {
        qCritical() << "Session invalid. Error: " << errorMessage(session.error());
    }
    
    return a.exec();
}
