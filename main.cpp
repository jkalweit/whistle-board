#include <QApplication>
#include <QtQml>
#include <QQmlApplicationEngine>
#include "App.h"
#include "WhistleListener.h"

int main(int argc, char *argv[])
{

    qmlRegisterType<App>("WhistleBoard", 1, 0, "App");
    qmlRegisterType<WhistleListener>("WhistleBoard", 1, 0, "WhistleListener");
    qmlRegisterType<DFTOutput>("WhistleBoard", 1, 0, "DFTOutput");
    qmlRegisterType<FrequencyRange>("WhistleBoard", 1, 0, "FrequencyRange");


    App* whistleApp = new App();

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qml"));
    engine.rootContext()->setContextProperty("app", (QObject*)whistleApp);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    whistleApp->doTest();

    return app.exec();
}
