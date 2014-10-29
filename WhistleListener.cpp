#include "WhistleListener.h"
#include <QThread>
#include <QDebug>
#include <qendian.h>
#include <qmath.h>
#include <math.h>

WhistleListener::WhistleListener(QObject *parent) :
    QIODevice(parent)
{
    m_N = 512;
    m_currIndex = 0;

    m_window = new double[m_N];
    for(int i = 0; i < m_N; i++) {
      m_window[i] = 0.54 - (0.46 * qCos((2 * M_PI * i) / (m_N - 1)));
    }

    m_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*m_N);
    //m_inWindowed = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*m_N);
    m_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*m_N);
    m_plan = fftw_plan_dft_1d(m_N, m_in, m_out, FFTW_FORWARD, FFTW_ESTIMATE);

    for(int i = 0; i < m_N / 2; i++) {
        m_outputs.append(new DFTOutput());
    }

    m_historyDepth = 2;
    m_history = new double*[m_N / 2];
    for(int i = 0; i < m_N / 2; i++) {
        m_history[i] = new double[m_historyDepth];
    }

    m_bucketSize = 32;
    m_numBuckets = m_N / m_bucketSize;

    for(int i = 0; i < m_numBuckets; i++) {
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
//    qWarning() << "bytesReady = " << m_audioInput->bytesReady()
//               << ", " << "elapsedUSecs = " <<m_audioInput->elapsedUSecs()
//               << ", " << "processedUSecs = "<<m_audioInput->processedUSecs();
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

            m_in[m_currIndex][0] = m_window[m_currIndex] * value;
            m_currIndex++;

            if(m_currIndex >= m_N) {
                m_currIndex = 0;

                fftw_execute(m_plan);

                double smoothed[m_N / 2];
                for(int i = 2; i < (m_N / 2) - 3; i++) {
                    smoothed[i] = (abs(m_out[i - 2][0]) + abs(m_out[i - 1][0]) + abs(m_out[i][0]) + abs(m_out[i + 1][0]) + abs(m_out[i + 2][0])) / 5;

                    for(int j = 1; j < m_historyDepth; j++) {
                        m_history[i][j-1] = m_history[i][j];
                    }
                    m_history[i][m_historyDepth-1] = smoothed[i];
                }

                double avg;
                for(int i = 0; i < (m_N / 2); i++) {
                    avg = 0;
                    for(int j = 0; j < m_historyDepth; j++) {
                        avg += m_history[i][j];
                    }
                    smoothed[i] = avg / m_historyDepth;
                }

                double temp[m_numBuckets];
                for(int i = 0; i < m_numBuckets; i++) {
                    temp[i] = 0;
                }



                for(int i = 0; i < m_N / 2; i++) {                                        
                    m_outputs[i]->setValue(smoothed[i] / 500); //smoothed[i] / 100); // m_out[i][0] / 10);
                    int bucket = i / m_numBuckets;
                    temp[bucket] += smoothed[i] / 100; // (smoothed[i]);
                }

                for(int i = 0; i < m_numBuckets; i++) {
                    m_frequencyRanges[i]->setValue(temp[i] / m_bucketSize);
                }
            }




            maxValue = qMax(value, maxValue);
            ptr += channelBytes;
        }
    }

    return len;
}

QQmlListProperty<DFTOutput> WhistleListener::outputs() {
    m_outputsFiltered = m_outputs.mid(32); // hacked bandpass filter :)
    return QQmlListProperty<DFTOutput>(this, m_outputsFiltered);
}

QQmlListProperty<FrequencyRange> WhistleListener::frequencies() {
    m_frequencyRangesFiltered = m_frequencyRanges.mid(2);  // hacked bandpass filter :)
    return QQmlListProperty<FrequencyRange>(this, m_frequencyRangesFiltered);
}
