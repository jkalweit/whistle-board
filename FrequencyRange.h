#ifndef FREQUENCYRANGE_H
#define FREQUENCYRANGE_H

#include <QObject>

class FrequencyRange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value MEMBER m_value WRITE setValue NOTIFY valueChanged)
public:
    explicit FrequencyRange(QObject *parent = 0);

    void setValue(double value);

signals:
    void valueChanged(double);

public slots:

private:
    double m_value;

};

#endif // FREQUENCYRANGE_H
