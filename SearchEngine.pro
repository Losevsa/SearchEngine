QT       += core gui concurrent testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    answer.cpp \
    dictionary.cpp \
    main.cpp \
    request.cpp \
    searchengine.cpp \
    testdictionary.cpp

HEADERS += \
    answer.h \
    dictionary.h \
    request.h \
    searchengine.h \
    testdictionary.h

FORMS += \
    searchengine.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    files/answers.json \
    files/config.json \
    files/data.json \
    files/file001.txt \
    files/file002.txt \
    files/requests.json
