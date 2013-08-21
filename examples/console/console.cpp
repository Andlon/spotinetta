#include "console.h"

#include <Spotinetta/playlist.h>
#include <Spotinetta/link.h>
#include <Spotinetta/track.h>
#include <Spotinetta/watchers.h>

#include <QRegularExpression>
#include <QStringList>

#include <QCoreApplication>
#include <QAudioOutput>

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

AudioOutput::~AudioOutput()
{
    if (!m_output.isNull())
        m_output->deleteLater();
}

int AudioOutput::deliver(const Spotinetta::AudioFrameCollection &collection)
{
    if (m_output.isNull())
    {
        m_output = new QAudioOutput(collection.format());

        m_output->setBufferSize(100000000);
        m_device = m_output->start();

        //qDebug() << "Buffer size: " << m_output->bufferSize();

        QObject::connect(m_output, &QAudioOutput::stateChanged, [] (QAudio::State state) {
            qDebug() << state;
        });
    }

    if (m_output->format() == collection.format())
    {
        qDebug() << m_output->bytesFree();
        qint64 toWrite = qMin(m_output->bytesFree(), collection.bytes());

        // Correct toWrite so that it aligns with frame boundaries
        toWrite -= (toWrite % m_output->format().bytesPerFrame());

        qint64 written = m_device->write(collection.data(), toWrite);
        Q_ASSERT(written == toWrite);

        int frames = toWrite / m_output->format().bytesPerFrame();

        //qDebug() << "Consumed " << frames << "frames";
        return frames;
    }
    else if (m_output->state() == QAudio::IdleState)
    {
        m_output->deleteLater();
        m_output = new QAudioOutput(collection.format());
        m_device = m_output->start();
    }

    return 0;
}

void AudioOutput::reset()
{
    m_output->reset();
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
                m_session->load(track);
                m_session->play();
            }
            else
            {
                out << "play: Invalid track link." << endl;
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
