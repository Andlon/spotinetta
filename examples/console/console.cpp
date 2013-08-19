#include "console.h"

#include <Spotinetta/playlist.h>
#include <Spotinetta/playlistcontainerwatcher.h>

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

Console::Console(Spotinetta::Session *session, QObject *parent)
    : QObject(parent), m_session(session), in(stdin), out(stdout)
{
    using Spotinetta::Session;
    connect(session, &Session::loggedIn, this, &Console::onLoggedIn);
    connect(session, &Session::loginFailed, this, &Console::onLoginFailed);
}

void Console::start()
{
    QString username, password;

    out << "Username: " << flush;
    in >> username;

    out << "Password: " << flush;
    in >> password;
    out << endl;

    out << clear << "Logging in..." << endl;

    m_session->login(username, password);
}

void Console::onLoggedIn()
{
    out << "Successfully logged in. " << endl;

    // Load root playlist container
    sp::PlaylistContainerWatcher * watcher = new sp::PlaylistContainerWatcher(m_session, this);
    connect(watcher, &sp::PlaylistContainerWatcher::loaded, [=] {
        out << "Available playlists: " << endl;
        sp::PlaylistContainer container = watcher->watched();
        sp::PlaylistList playlists = container.playlists();
        for (const sp::Playlist &playlist : playlists)
        {
            out << playlist.name() << " (" << playlist.trackCount() << ")" << endl;
        }
    });

    connect(watcher, &sp::PlaylistContainerWatcher::loaded, watcher, &sp::PlaylistContainerWatcher::deleteLater);

    watcher->watch(m_session->rootContainer());
}

void Console::onLoginFailed(Spotinetta::Error error)
{
    out << "Login failed. Error: " << Spotinetta::errorMessage(error) << endl;
}
