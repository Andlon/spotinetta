#pragma once

#include <libspotify/api.h>
#include <QtGlobal>

namespace Spotinetta {

class AudioFormat {
public:
    enum class SampleType {
        Int16NativeEndian = SP_SAMPLETYPE_INT16_NATIVE_ENDIAN
    };

    AudioFormat(int sampleRate, int channels, SampleType sampleType = SampleType::Int16NativeEndian)
        :   m_sampleRate(sampleRate), m_channels(channels), m_sampleType(sampleType)
    {
        Q_ASSERT_X(m_sampleType == SampleType::Int16NativeEndian,
                   "Spotinetta::AudioFormat", "SampleType not supported.");
    }

    SampleType sampleType() const { return m_sampleType; }
    int sampleRate() const { return m_sampleRate; }
    int channels() const { return m_channels; }

    int bytesPerFrame() const { return 2 * m_channels; }

private:
    SampleType  m_sampleType;
    int         m_sampleRate;
    int         m_channels;
};

}
