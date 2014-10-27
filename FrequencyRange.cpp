#include "FrequencyRange.h"

FrequencyRange::FrequencyRange(QObject *parent) :
    QObject(parent), m_value(0)
{
}

void FrequencyRange::setValue(double value) {
    if(m_value != value) {
        m_value = value;
        valueChanged(m_value);
    }
}
