#pragma once

#include "audioformat.h"
#include "detail/ringbuffer.h"

#include <QObject>
#include <QQueue>
#include <QMutex>

namespace Spotinetta {



class AudioBuffer : public QObject {
    Q_OBJECT

public:
    explicit AudioBuffer(qint64 size, QObject * parent = 0);

    int                     deliver(const QByteArray &data, const AudioFrameCollection &collection);

    AudioFrameCollection    peekCollection(int maxBytes = 0) const;
    void                    takeCollection(QByteArray &buffer, const AudioFrameCollection &collection);

signals:
    void dataAvailable();

private:
    detail::RingBuffer<char, 2048>  m_buffer;
    QQueue<AudioFrameCollection>    m_collections;
    mutable QMutex                  m_queueMutex;

};

}
