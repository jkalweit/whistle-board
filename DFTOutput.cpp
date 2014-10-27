#include "DFTOutput.h"

DFTOutput::DFTOutput(QObject *parent) :
    QObject(parent), m_value(0)
{
}


void DFTOutput::setValue(double value) {
    if(m_value != value) {
        m_value = value;
        valueChanged(m_value);
    }
}

