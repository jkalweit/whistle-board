#ifndef WHISTLELISTENER_H
#define WHISTLELISTENER_H

#include <QObject>
#include <QAudioInput>
#include <QByteArray>
#include <QQmlListProperty>
#include <fftw3.h>
#include "AudioInput.h"
#include "FrequencyRange.h"


class WhistleListener : public QIODevice
{
    Q_OBJECT
    Q_PROPERTY(int N MEMBER m_N NOTIFY nChanged)
    Q_PROPERTY(QQmlListProperty<FrequencyRange> frequencies READ frequencies NOTIFY frequenciesChanged)
public:
    explicit WhistleListener(QObject *parent = 0);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    QQmlListProperty<FrequencyRange> frequencies();

signals:
    void nChanged(int);
    void frequenciesChanged(QQmlListProperty<FrequencyRange>);

public slots:
    void startListening();
    void notified();

private:
    void initializeAudio();
    void createAudioInput();

    QAudioDeviceInfo m_device;
    QAudioFormat m_format;
    QAudioInput *m_audioInput;
    QIODevice *m_input;
    bool m_pullMode;
    QByteArray m_buffer;


    int m_N;
    int m_currIndex;
    fftw_complex* m_in;
    fftw_complex* m_out;
    fftw_plan m_plan;


    QList<FrequencyRange*> m_frequencyRanges;
};

#endif // WHISTLELISTENER_H
