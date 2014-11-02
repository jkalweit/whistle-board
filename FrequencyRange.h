#ifndef FREQUENCYRANGE_H
#define FREQUENCYRANGE_H

#include <QObject>
#include <QStringList>

class FrequencyRange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value MEMBER m_value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QStringList characters MEMBER m_characters NOTIFY charactersChanged)
    Q_PROPERTY(bool isActive MEMBER m_isActive WRITE setIsActive NOTIFY isActiveChanged)
public:
    explicit FrequencyRange(QObject *parent = 0);

    double value();
    void setValue(double);
    void setIsActive(bool);

    void addCharacter(QString character);
    Q_INVOKABLE QString getCharacter(int index);

signals:
    void valueChanged(double);
    void charactersChanged(QStringList);
    void isActiveChanged(bool);

public slots:

private:
    double m_value;
    QStringList m_characters;
    bool m_isActive;
};

#endif // FREQUENCYRANGE_H
