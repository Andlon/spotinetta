#include <Spotinetta/audiobuffer.h>

#include <QMutexLocker>

namespace Spotinetta {

AudioFrameCollection::AudioFrameCollection(int bytes, const AudioFormat &format)
    :   m_bytes(bytes), m_format(format)
{
    Q_ASSERT(bytes % m_format.bytesPerFrame() == 0);
}

int AudioFrameCollection::bytes() const
{
    return m_bytes;
}

int AudioFrameCollection::frames() const
{
    return m_bytes / m_format.bytesPerFrame();
}

AudioBuffer::AudioBuffer(qint64 size, QObject *parent)
    :   QObject(parent), m_buffer(size)
{ }

int AudioBuffer::deliver(const QByteArray &data, const AudioFrameCollection &collection)
{
    qint64 available = m_buffer.free();
    qint64 toWrite = qMin(available, (qint64) data.size());

    // Make sure toWrite contains a number evenly divisible by the number of bytes per frame
    int bytesPerFrame = collection.format().bytesPerFrame();
    toWrite -= (toWrite % bytesPerFrame);

    qint64 written = m_buffer.write(data.constData(), toWrite);

    QMutexLocker locker(&m_queueMutex);
    m_collections.enqueue(AudioFrameCollection(toWrite, collection.format()));

    // written SHOULD be equal to toWrite, or something's gone wrong
    Q_ASSERT(written == toWrite);

    return toWrite;
}

AudioFrameCollection AudioBuffer::peekCollection(int maxBytes) const
{
    AudioFrameCollection collection;
    int collectedBytes = 0;

    {
        QMutexLocker locker(&m_queueMutex);

        if (!m_collections.empty())
        {
            // Read first element
        }

        while (collectedBytes < maxBytes && !m_collections.empty())
        {
            AudioFrameCollection afc = m_collections.head();
        }
    }

    return collection;
}


}
