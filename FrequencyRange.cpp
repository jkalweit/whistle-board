#include "FrequencyRange.h"


FrequencyRange::FrequencyRange(QObject *parent) :
    QObject(parent), m_value(0), m_isActive(false)
{
}

double FrequencyRange::value() {
    return m_value;
}

void FrequencyRange::setValue(double value) {
    if(m_value != value) {
        m_value = value;
        valueChanged(m_value);
    }
}

void FrequencyRange::setIsActive(bool isActive) {
    if(m_isActive != isActive) {
        m_isActive = isActive;
        isActiveChanged(m_isActive);
    }
}

void FrequencyRange::addCharacter(QString character) {
    m_characters.append(character);
}

QString FrequencyRange::getCharacter(int index) {
    return m_characters[index % m_characters.length()];
}
