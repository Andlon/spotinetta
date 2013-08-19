#pragma once
#include <QObject>

template <class T> class QFutureWatcher;
class QTextStream;

class ConsoleInput : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleInput(QObject *parent = 0);

    bool acceptsInput() const;
    void setAcceptsInput(bool accepts);

signals:
    void input(const QString &line);

private:
    void collectInput();

    bool                        m_accept;
    QFutureWatcher<QString> *   m_watcher;
};
