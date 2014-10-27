#include "WhistleListener.h"
#include <QThread>
#include <QDebug>
#include <qendian.h>


WhistleListener::WhistleListener(QObject *parent) :
    QIODevice(parent)
{
    m_N = 256;
    m_currIndex = 0;
    m_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*m_N);
    m_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*m_N);
    m_plan = fftw_plan_dft_1d(m_N, m_in, m_out, FFTW_FORWARD, FFTW_ESTIMATE);



    for(int i = 0; i < m_N / 2; i++) {
        m_frequencyRanges.append(new FrequencyRange());
    }
}


void WhistleListener::start()
{
    open(QIODevice::WriteOnly);
}

void WhistleListener::stop()
{
    close();
}

void WhistleListener::startListening() {
    initializeAudio();
    createAudioInput();
}

void WhistleListener::initializeAudio() {

    m_device = QAudioDeviceInfo::defaultInputDevice();

    m_format.setSampleRate(8000);
    m_format.setChannelCount(1);
    m_format.setSampleSize(16);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }
}

void WhistleListener::createAudioInput()
{
    m_audioInput = new QAudioInput(m_device, m_format, this);
    connect(m_audioInput, SIGNAL(notify()), SLOT(notified()));
    //connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)), SLOT(handleStateChanged(QAudio::State)));
    start();
    m_audioInput->start(this);
}

void WhistleListener::notified()
{
    qWarning() << "bytesReady = " << m_audioInput->bytesReady()
               << ", " << "elapsedUSecs = " <<m_audioInput->elapsedUSecs()
               << ", " << "processedUSecs = "<<m_audioInput->processedUSecs();
}



qint64 WhistleListener::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 WhistleListener::writeData(const char *data, qint64 len)
{
    Q_ASSERT(m_format.sampleSize() % 8 == 0);
    const int channelBytes = m_format.sampleSize() / 8;
    const int sampleBytes = m_format.channelCount() * channelBytes;
    Q_ASSERT(len % sampleBytes == 0);
    const int numSamples = len / sampleBytes;

    quint32 maxValue = 0;
    const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < m_format.channelCount(); ++j) {
            quint32 value = 0;            

            if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                value = *reinterpret_cast<const quint8*>(ptr);
            } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                value = qAbs(*reinterpret_cast<const qint8*>(ptr));
            } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qFromLittleEndian<quint16>(ptr);
                else
                    value = qFromBigEndian<quint16>(ptr);
            } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qAbs(qFromLittleEndian<qint16>(ptr));
                else
                    value = qAbs(qFromBigEndian<qint16>(ptr));
            } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qFromLittleEndian<quint32>(ptr);
                else
                    value = qFromBigEndian<quint32>(ptr);
            } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
                if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                    value = qAbs(qFromLittleEndian<qint32>(ptr));
                else
                    value = qAbs(qFromBigEndian<qint32>(ptr));
            } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
                value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
            }

            m_in[m_currIndex++][0] = value;
            if(m_currIndex >= m_N) {
                m_currIndex = 0;
                fftw_execute(m_plan);
                for(int i = 0; i < m_N / 2; i++) {
                    m_frequencyRanges[i]->setValue(m_out[i][0] / 100);
                }
            }




            maxValue = qMax(value, maxValue);
            ptr += channelBytes;
        }
    }

    return len;
}



QQmlListProperty<FrequencyRange> WhistleListener::frequencies() {
    return QQmlListProperty<FrequencyRange>(this, m_frequencyRanges);
}
