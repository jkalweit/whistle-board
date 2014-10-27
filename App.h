#ifndef APP_H
#define APP_H

#include <QList>
#include <QObject>
#include <QThread>
#include "AudioInput.h"
#include "WhistleListener.h"

class App : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WhistleListener* whistleListener MEMBER m_listener NOTIFY whistleListenerChanged)
public:
    explicit App(QObject *parent = 0);    

    Q_INVOKABLE void doTest();

signals:
    void whistleListenerChanged(WhistleListener*);

public slots:

private:
    WhistleListener* m_listener;

};

#endif // APP_H
