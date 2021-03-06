#pragma once

#include <QtGlobal>


#ifdef QT_MULTIMEDIA_LIB
#include <QtMultimedia/QAudioFormat>
#endif

namespace Spotinetta {

class AudioFormat {
public:
    enum class SampleType {
        Int16NativeEndian = SP_SAMPLETYPE_INT16_NATIVE_ENDIAN
    };

    AudioFormat() : m_sampleType(SampleType::Int16NativeEndian), m_sampleRate(0), m_channels(0) { }
    AudioFormat(int sampleRate, int channels, SampleType sampleType = SampleType::Int16NativeEndian)
        :   m_sampleType(sampleType), m_sampleRate(sampleRate), m_channels(channels)
    {
        Q_ASSERT_X(m_sampleType == SampleType::Int16NativeEndian,
                   "Spotinetta::AudioFormat", "SampleType not supported.");
    }

    SampleType sampleType() const { return m_sampleType; }
    int sampleRate() const { return m_sampleRate; }
    int channels() const { return m_channels; }

    int bytesPerFrame() const { return 2 * m_channels; }

#ifdef QT_MULTIMEDIA_LIB
    operator QAudioFormat() const {
        QAudioFormat format;
        format.setChannelCount(m_channels);
        format.setSampleSize(16);
        format.setSampleRate(m_sampleRate);
        format.setSampleType(QAudioFormat::SignedInt);
        format.setCodec("audio/pcm");
        return format;
    }
#endif

private:
    SampleType  m_sampleType;
    int         m_sampleRate;
    int         m_channels;

    friend bool operator == (const AudioFormat &format1, const AudioFormat &format2);

#ifdef QT_MULTIMEDIA_LIB
    friend bool operator == (const AudioFormat &af, const QAudioFormat &qaf);
#endif
};

class AudioFrameCollection {
public:
    AudioFrameCollection(const char * data, int bytes, const AudioFormat &format);

    const char *    data() const { return m_data; }
    int             bytes() const { return m_bytes; }
    int             frames() const { return m_bytes / m_format.bytesPerFrame(); }
    AudioFormat     format() const { return m_format; }

private:
    const char *        m_data;
    const int           m_bytes;
    const AudioFormat   m_format;
};

class AudioOutputInterface {
public:
    virtual ~AudioOutputInterface() { }

    /**
     * @brief deliver Delivers music data from an internal libspotify thread.
     * @param collection An AudioFrameCollection containing the data and format description.
     * @return The number of _frames_ consumed.
     *
     */
    virtual int deliver(const AudioFrameCollection &collection) = 0;

    /**
     * @brief reset Called from an internal libspotify thread (the same as deliver() is called from).
     *              Signals that any buffers containing audiodata should be cleared.
     *
     */
    virtual void reset() = 0;
};


inline bool operator == (const AudioFormat &format1, const AudioFormat &format2)
{
    return format1.m_sampleType == format2.sampleType()
        && format1.m_sampleRate == format2.sampleRate()
        && format1.m_channels == format2.m_channels;
}

inline AudioFrameCollection::AudioFrameCollection(const char *data, int bytes, const AudioFormat &format)
    : m_data(data), m_bytes(bytes), m_format(format)
{
    Q_ASSERT_X(bytes % m_format.bytesPerFrame() == 0,
               "AudioFrameCollection::AudioFrameCollection",
               "Number of bytes delivered must be divisible by the amount of bytes per frame." );
}

#ifdef QT_MULTIMEDIA_LIB
inline bool operator == (const AudioFormat &af, const QAudioFormat &qaf)
{
    return af.bytesPerFrame() == qaf.bytesPerFrame()
        && af.channels() == qaf.channelCount()
        && af.sampleRate() == qaf.sampleRate()
        && qaf.sampleType() == QAudioFormat::SignedInt
        && qaf.codec() == "audio/pcm";
}
#endif

}
