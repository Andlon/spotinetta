#include "spotinettaconsole.h"

#include <iostream>

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

SpotinettaConsole::SpotinettaConsole(Spotinetta::Session *session, QObject *parent)
    : QObject(parent), m_session(session), in(stdin), out(stdout)
{
    using Spotinetta::Session;
    connect(session, &Session::loggedIn, this, &SpotinettaConsole::onLoggedIn);
    connect(session, &Session::loginFailed, this, &SpotinettaConsole::onLoginFailed);
}

void SpotinettaConsole::start()
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

void SpotinettaConsole::onLoggedIn()
{
    out << "Successfully logged in. " << endl;
}

void SpotinettaConsole::onLoginFailed(Spotinetta::Error error)
{
    out << "Login failed. Error: " << Spotinetta::errorMessage(error) << endl;
}