#include "console.h"

#include <Spotinetta/playlist.h>
#include <Spotinetta/playlistcontainerwatcher.h>
#include <Spotinetta/playlistwatcher.h>
#include <Spotinetta/link.h>
#include <Spotinetta/track.h>
#include <Spotinetta/watchers.h>

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

    sp::Link trackLink("spotify:track:5T6hai5yeaakZcs1QE8rFD");
    sp::Track track = trackLink.track();

    if (track.isValid())
    {
        out << "Loading track..." << endl;

        sp::TrackWatcher * watcher = new sp::TrackWatcher(m_session, this);
        connect(watcher, &sp::TrackWatcher::loaded, [=] {
            out << "Title: " << track.name() << endl
                << "Duration: " << track.duration() << endl;
        });

        connect(watcher, &sp::TrackWatcher::loaded, watcher, &sp::TrackWatcher::deleteLater);
        watcher->watch(track);
    }
    else
    {
        out << "Invalid track." << endl;
    }



//    auto printPlaylists = [this] (const sp::PlaylistContainer &container) {
//        sp::PlaylistList playlists = container.playlists();
//        for (const sp::Playlist &playlist : playlists)
//        {
//            out << playlist.name() << " (" << playlist.trackCount() << ")" << endl;
//        }
//    };

//    // Load root playlist container
//    sp::PlaylistContainerWatcher * watcher = new sp::PlaylistContainerWatcher(m_session, this);
//    connect(watcher, &sp::PlaylistContainerWatcher::loaded, [=] {
//        out << "Available playlists: " << endl;
//        sp::PlaylistContainer container = watcher->watched();
//        sp::PlaylistList playlists = container.playlists();

//        auto checkAndPrintPlaylists = [=] {
//            bool allLoaded = true;

//            sp::PlaylistList playlists = container.playlists();
//            for (const sp::Playlist &playlist : playlists)
//            {
//                if (!playlist.isLoaded())
//                    allLoaded = false;
//            }

//            if (allLoaded)
//                printPlaylists(container);
//        };

//        bool allLoaded = true;

//        for (const sp::Playlist &playlist : playlists)
//        {
//            if (!playlist.isLoaded())
//            {
//                allLoaded = false;
//                sp::PlaylistWatcher * playlistWatcher = new sp::PlaylistWatcher(m_session, this);
//                playlistWatcher->watch(playlist);
//                connect(playlistWatcher, &sp::PlaylistWatcher::stateChanged, [=] {
//                    if (playlistWatcher->watched().isLoaded())
//                    {
//                        playlistWatcher->deleteLater();
//                        checkAndPrintPlaylists();
//                    }
//                });
//            }
//        }

//        if (allLoaded)
//            printPlaylists(container);
//    });

//    connect(watcher, &sp::PlaylistContainerWatcher::loaded, watcher, &sp::PlaylistContainerWatcher::deleteLater);

//    watcher->watch(m_session->rootContainer());
}

void Console::onLoginFailed(Spotinetta::Error error)
{
    out << "Login failed. Error: " << Spotinetta::errorMessage(error) << endl;
}
