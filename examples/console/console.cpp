#include "console.h"

#include <Spotinetta/playlist.h>
#include <Spotinetta/link.h>
#include <Spotinetta/track.h>
#include <Spotinetta/watchers.h>

#include <QRegularExpression>
#include <QStringList>

#include <QCoreApplication>
#include <QAudioOutput>
#include <QMetaMethod>

#include <QDebug>

#include <iostream>

namespace sp = Spotinetta;

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

QTextStream & clear(QTextStream &stream)
{
#if defined(Q_OS_WIN)
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);

    FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);

    SetConsoleCursorPosition(hStdOut, coord);
#else
    stream << "\033[2J\033[1;1H";
#endif
    return stream;
}

namespace {
QStringList wordify(const QString &line)
{
    return line.split(QRegularExpression("[\\s]"), QString::SkipEmptyParts);
}
}

AudioOutput::AudioOutput(QObject *parent)
    :   QObject(parent), m_buffer(50000)
{

}

AudioOutput::~AudioOutput()
{
}

int AudioOutput::deliver(const Spotinetta::AudioFrameCollection &collection)
{
    sp::AudioFormat newFormat = collection.format();

    bool formatsEqual;

    {
        QMutexLocker locker(&m_formatLock);
        formatsEqual = newFormat == m_format;

        if (!formatsEqual && m_buffer.used() == 0)
        {
            m_format = newFormat;
        }
    }

    if (formatsEqual)
    {
        qint64 bytesPerFrame = newFormat.bytesPerFrame();
        qint64 toWrite = qMin(collection.bytes(), (int) m_buffer.free());
        toWrite -= (toWrite % bytesPerFrame);

        qint64 written = m_buffer.write(collection.data(), toWrite);
        Q_ASSERT(written == toWrite);

        // Replace SLOT macro way of calling with a more modern function pointer approach
        QMetaObject::invokeMethod(this, "push", Qt::QueuedConnection);

        // Return number of frames consumed
        return toWrite / bytesPerFrame;
    }

    return 0;
}

void AudioOutput::reset()
{
    qDebug() << "Should reset.";
}

void AudioOutput::push()
{
    QAudioFormat format;
    {
        QMutexLocker locker(&m_formatLock);
        format = m_format;
    }

    if (m_output.isNull())
    {
        setupOutput(format);
    }

    //qDebug() << m_buffer.used() << "\t:\t" << (m_output->bufferSize() - m_output->bytesFree());

    if (m_output->format() == format)
    {
        qint64 bytesPerFrame = format.bytesPerFrame();
        qint64 toRead = qMin(m_buffer.used(), (qint64) m_output->bytesFree());

        // Adjust toRead for frame boundaries
        toRead -= (toRead % bytesPerFrame);

        QByteArray data;
        data.resize(toRead);
        qint64 read = m_buffer.read(data.data(), data.size());
        qint64 written = m_device->write(data);

        Q_ASSERT(read == toRead);
        Q_ASSERT(written == read);
    }
    else if (m_output->state() != QAudio::ActiveState)
    {
        m_output->deleteLater();
        m_output.clear();
        setupOutput(format);
    }
}

void AudioOutput::setupOutput(const QAudioFormat &format)
{
    m_output = new QAudioOutput(format, this);
    m_device = m_output->start();

    int notifyMs = format.durationForBytes(m_output->bufferSize() / 2) / 1000;
    m_output->setNotifyInterval(notifyMs);

    connect(m_output, &QAudioOutput::notify, this, &AudioOutput::push);
}

Console::Console(Spotinetta::Session *session, QObject *parent)
    : QObject(parent), m_session(session), m_state(State::Idle), in(new ConsoleInput(this)), out(stdout)
{
    connect(session, &sp::Session::loggedIn, this, &Console::onLoggedIn);
    connect(session, &sp::Session::loggedOut, this, &Console::onLoggedOut);
    connect(session, &sp::Session::loginFailed, this, &Console::onLoginFailed);

    connect(in, &ConsoleInput::input, this, &Console::onConsoleInput);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this] {
        out << "Exiting..." << endl;
    });
}

void Console::start()
{
    unblock();
}

void Console::onLoggedIn()
{
    out << "Successfully logged in. " << endl;
    processState();
}

void Console::onLoggedOut()
{
    out << "Logged out. " << endl;
    processState();
}

void Console::onConsoleInput(const QString &line)
{
    const QStringList words = wordify(line);

    if (words.isEmpty())
        return;

    switch (m_state)
    {
    case (State::Idle):
        processIdleInput(words);
        break;
    case (State::LoginUsername):
        m_loginUsername = line.simplified();
        m_state = State::LoginPassword;
        processState();
        break;
    case (State::LoginPassword):
        m_loginPassword = line.simplified();
        login();
        break;

    default:
        break;
    }
}

void Console::onLoginFailed(Spotinetta::Error error)
{
    out << "Login failed. Error: " << Spotinetta::errorMessage(error) << endl;
    processState();
}

void Console::onLogMessage(const QString &message)
{
    out << message.simplified() << endl;
}

void Console::login()
{
    m_session->login(m_loginUsername, m_loginPassword);
    m_loginUsername.clear();
    m_loginPassword.clear();
    block();
    m_state = State::Waiting;
}

void Console::block()
{
    in->setAcceptsInput(false);
}

void Console::unblock()
{
    in->setAcceptsInput(true);
}

void Console::processIdleInput(const QStringList &words)
{
    const QString command = words.at(0).toLower();

    if (command == "login")
    {
        m_state = State::LoginUsername;

        if (words.count() >= 2)
        {
            m_loginUsername = words.at(1);
            m_state = State::LoginPassword;
        }

        if (words.count() >= 3)
        {
            m_loginPassword = words.at(2);
            login();
        }

        processState();
    }
    else if (command == "exit" || command == "quit")
        quit();
    else if (command == "logout")
    {
        logout();
    }
    else if (command == "log")
    {
        if (words.count() >= 2)
        {
            const QString toggle = words.at(1).toLower();
            if (toggle == "on")
            {
                connect(m_session, &sp::Session::log, this, &Console::onLogMessage, Qt::UniqueConnection);
            }
            else if (toggle == "off")
            {
                disconnect(m_session, &sp::Session::log, this, &Console::onLogMessage);
            }
            else if (toggle == "out")
            {
                // Let user log out by typing both "logout" and "log out"
                logout();
            }
        }
    }
    else if (command == "play")
    {
        if (words.count() >= 2)
        {
            const QString url = words.at(1);
            sp::Link link(url);
            sp::Track track = link.track();

            if (track.isValid())
            {
                auto playTrack = [=] {
                    m_session->load(track);
                    m_session->play();
                    m_session->seek(0);
                    out << "Playing: " << track.name() << endl;
                };

                if (!track.isLoaded())
                {
                    sp::TrackWatcher * watcher = new sp::TrackWatcher(m_session, this);
                    connect(watcher, &sp::TrackWatcher::loaded, playTrack);
                    connect(watcher, &sp::TrackWatcher::loaded, watcher, &sp::TrackWatcher::deleteLater);
                    watcher->watch(track);
                }
                else
                {
                    playTrack();
                }
            }
            else
            {
                out << "play: Invalid track link." << endl;
            }
        }
        else
        {
            m_session->play();
        }
    }
    else if (command == "pause")
    {
        m_session->pause();
    }
    else if (command == "seek")
    {
        if (words.count() >= 2)
        {
            QString strNum = words.at(1);
            bool ok;
            int sec = strNum.toInt(&ok);

            if (ok)
            {
                m_session->seek(sec * 1000);
            }
        }
    }
}

void Console::processState()
{
    switch (m_state)
    {
    case (State::Idle):
        unblock();
        break;
    case (State::Waiting):
        unblock();
        m_state = State::Idle;
        break;
    case (State::Quitting):
        QCoreApplication::quit();
        break;
    case (State::LoginUsername):
        out << "Username: " << flush;
        break;
    case (State::LoginPassword):
        out << "Password: " << flush;
        break;

    default:
        break;
    }
}

void Console::logout()
{
    if (m_session->connectionState() == sp::Session::ConnectionState::LoggedIn)
    {
        m_session->logout();
        m_state = State::Waiting;
        block();
    }
}

void Console::quit()
{
    block();
    m_state = State::Quitting;

    if (m_session->connectionState() == sp::Session::ConnectionState::LoggedIn)
        m_session->logout();
    else
        QCoreApplication::quit();
}
