CONFIG += c++11

TEMPLATE = app

QT += qml quick widgets multimedia

LIBS += -lfftw3

SOURCES += main.cpp \
    App.cpp \
    FrequencyRange.cpp \
    AudioInput.cpp \
    WhistleListener.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    App.h \
    FrequencyRange.h \
    AudioInput.h \
    WhistleListener.h
