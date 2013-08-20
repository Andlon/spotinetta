#pragma once

#include "audioformat.h"

#include <QObject>
#include <libspotify/api.h>


namespace Spotinetta {

class AudioFrameCollection {
public:
    AudioFrameCollection(const char * data, int bytes, const AudioFormat &format);

    const char *    data() const;

    int             bytes() const;
    int             frames() const;
    AudioFormat     format() const;

private:
    QByteArray  m_data;
    AudioFormat m_format;

    friend class AudioBuffer;
};

class AudioBuffer : public QObject {
    Q_OBJECT

public:
    explicit AudioBuffer(QObject * parent = 0);

    int                     deliver(const AudioFrameCollection &collection);
    AudioFrameCollection    peek(int maxBytes = 0) const;
    void                    consume(AudioFrameCollection &collection);

signals:
    void dataAvailable();

};

}
