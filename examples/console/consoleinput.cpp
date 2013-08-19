#include "consoleinput.h"

#include <iostream>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>

ConsoleInput::ConsoleInput(QObject *parent) :
    QObject(parent), m_accept(false)
{
    m_watcher = new QFutureWatcher<QString>(this);

    connect(m_watcher, &QFutureWatcher<QString>::finished, [this] {
        emit input(m_watcher->future().result());
        if (acceptsInput())
            collectInput();
    });
}

bool ConsoleInput::acceptsInput() const
{
    return m_accept;
}

void ConsoleInput::setAcceptsInput(bool accepts)
{
    if (m_accept != accepts)
    {
        m_accept = accepts;

        if (m_accept)
        {
            collectInput();
        }
    }
}

void ConsoleInput::collectInput()
{
    if (!m_watcher->isRunning())
    {
        QFuture<QString> future = QtConcurrent::run([] {
            QTextStream in(stdin);
            return in.readLine();
        });

        m_watcher->setFuture(future);
    }
}
