#include <Spotinetta/audiobuffer.h>

namespace Spotinetta {

AudioFrameCollection::AudioFrameCollection(const char *data, int size, const AudioFormat &format)
    :   m_data(QByteArray::fromRawData(data, size)), m_format(format)
{
    Q_ASSERT(m_data.size() % m_format.bytesPerFrame() == 0);
}

const char * AudioFrameCollection::data() const
{
    return m_data.constData();
}

int AudioFrameCollection::bytes() const
{
    return m_data.size();
}

int AudioFrameCollection::frames() const
{
    return m_data.size() / m_format.bytesPerFrame();
}


}
