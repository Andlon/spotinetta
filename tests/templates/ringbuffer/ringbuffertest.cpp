#include <QString>
#include <QtTest>
#include <QByteArray>

#include <Spotinetta/detail/ringbuffer.h>

//typedef Spotinetta::detail::RingBuffer RingBuffer;

class RingbufferTest : public QObject
{
    Q_OBJECT
    
public:
    RingbufferTest();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testInputOutput();
};

RingbufferTest::RingbufferTest()
{
}

void RingbufferTest::initTestCase()
{
}

void RingbufferTest::cleanupTestCase()
{
}

void RingbufferTest::testInputOutput()
{
    Spotinetta::detail::RingBuffer<char, 8> buffer(16);

    QByteArray input = "Hello world!";
    QByteArray output;
    output.resize(input.size());

    buffer.write(input.constData(), input.size());
    buffer.read(output.data(), output.size());

    QVERIFY(input == output);
}

QTEST_APPLESS_MAIN(RingbufferTest)

#include "ringbuffertest.moc"