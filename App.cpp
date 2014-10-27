#include "App.h"

#include <QDebug>

App::App(QObject *parent) :
    QObject(parent)
{

    m_listener = new WhistleListener(this);

}


void App::doTest() {

    qDebug() << "Doing test...";


    //QThread* thread = new QThread;
    //m_listener->moveToThread(thread);
    //thread->start();
    //QMetaObject::invokeMethod(m_listener, "startListening", Qt::QueuedConnection);
    m_listener->startListening();

//    for(int i = 0; i < N; i++) {
//        in[i][0] = 0;
//        in[i][1] = 0;
//    }

//    in[3][0] = 10;

//    fftw_execute(my_plan);

//    for(int i = 0; i < N; i++) {
//        m_frequencyRanges[i]->setValue(out[i][0]);
//    }

//    fftw_destroy_plan(my_plan);
//    fftw_free(in);
//    fftw_free(out);

}

