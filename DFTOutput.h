#ifndef DFTOUTPUT_H
#define DFTOUTPUT_H

#include <QObject>

class DFTOutput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value MEMBER m_value WRITE setValue NOTIFY valueChanged)
public:
    explicit DFTOutput(QObject *parent = 0);

    void setValue(double value);

signals:
    void valueChanged(double);

public slots:

private:
    double m_value;




};

#endif // DFTOUTPUT_H
