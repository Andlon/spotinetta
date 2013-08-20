#pragma once

#include <QScopedArrayPointer>
#include <QSemaphore>

namespace Spotinetta {

namespace detail {

template <class T, qint64 ChunkSize>
class RingBuffer {
public:
    explicit RingBuffer(qint64 size) :
        m_data(new T[size]), m_size(size),
        m_free(size), m_used(size),
        m_start(0), m_end(0) { m_free.acquire(size); }

    qint64 free() const;
    qint64 used() const;
    qint64 size() const;

    qint64 write(const T * data, qint64 maxSize);
    qint64 read(T * data, qint64 maxSize);

private:
    QScopedArrayPointer<T>  m_data;
    const qint64            m_size;

    QSemaphore m_free;
    QSemaphore m_used;

    qint64 m_start;
    qint64 m_end;
};

template <class T, qint64 ChunkSize>
inline qint64 RingBuffer<T, ChunkSize>::free() const
{
    return m_used.available();
}

template <class T, qint64 ChunkSize>
inline qint64 RingBuffer<T, ChunkSize>::used() const
{
    return m_free.available();
}

template <class T, qint64 ChunkSize>
inline qint64 RingBuffer<T, ChunkSize>::size() const
{
    return m_size;
}

template <class T, qint64 ChunkSize>
inline qint64 RingBuffer<T, ChunkSize>::write(const T * data, qint64 maxSize)
{
    // Determine the absolute maximum amount of objects to write
    qint64 maximum = qMin(maxSize, m_size);

    qint64 written = 0;
    qint64 pos = m_end;
    qint64 available = free();

    while (available > 0 && written < maximum)
    {
        // Determine how many objects to write based on the minimum
        // of avail, chunkSize and (maximum-written)
        qint64 toWrite = qMin(qMin(maximum - written, ChunkSize), available);

        m_used.acquire(toWrite);

        for (qint64 i = 0; i < toWrite; ++i)
        {
            m_data[pos] = data[written + i];

            // Make sure to correct position when it loops back
            // to the beginning of the buffer
            pos = (pos + 1) % m_size;
        }

        // Update end position
        m_end = (m_end + toWrite) % m_size;
        written += toWrite;

        m_free.release(toWrite);
        available = free();
    }

    return written;
}

template <class T, qint64 ChunkSize>
inline qint64 RingBuffer<T, ChunkSize>::read(T * data, qint64 maxSize)
{
    // Determine the absolute maximum of objects to read
    qint64 maximum = qMin(maxSize, m_size);

    qint64 read = 0;
    qint64 pos = m_start;
    qint64 available = used();

    while (available > 0 && read < maximum)
    {
        qint64 toRead = qMin(qMin(maximum - read, ChunkSize), available);

        m_free.acquire(toRead);

        for (qint64 i = 0; i < toRead; ++i)
        {
            data[read + i] = m_data[pos];
            pos = (pos + 1) % m_size;
        }

        m_start = (m_start + toRead) % m_size;
        read += toRead;
        m_used.release(toRead);
        available = used();
    }

    return read;
}



}
}
