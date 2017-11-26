QT += multimedia network
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
LIBS += -L$$PWD/3rdparity/azurestorage/bin -lazurestorage
LIBS += -L$$PWD/3rdparity/azurestorage/bin -lcpprest

INCLUDEPATH += $$PWD/3rdparity/azurestorage/includes
DEPENDPATH += $$PWD/3rdparity/azurestorage/includes

LIBS += -lboost_system -lcrypto -lssl
}

SOURCES += main.cpp \
    host.cpp \
    receiver.cpp \
    files.cpp

HEADERS += \
    host.h \
    receiver.h \
    files.h
