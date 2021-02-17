QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += CURL_STATICLIB

SOURCES += \
    aboutdialog.cpp \
    clickablelabel.cpp \
    gameinfo.cpp \
    httpsmanager.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    aboutdialog.h \
    clickablelabel.h \
    gameinfo.h \
    httpsmanager.h \
    widget.h

FORMS += \
    aboutdialog.ui \
    widget.ui \
    widget.ui

LIBS += -L$$PWD/libs/lib/ -llibcurl_a
PRE_TARGETDEPS += $$PWD/libs/lib/libcurl_a.lib
INCLUDEPATH += $$PWD/libs/include
DEPENDPATH += $$PWD/libs/include

VERSION = 1.0.0.0
RC_ICONS = logo.ico
QMAKE_TARGET_PRODUCT = LeagueTeamBoost
QMAKE_TARGET_COMPANY = Mario & ButterCookies
QMAKE_TARGET_DESCRIPTION = League of Legends Team Boost
QMAKE_TARGET_COPYRIGHT = Mario & ButterCookies

QMAKE_CXXFLAGS_RELEASE = -O2

RESOURCES += \
    resources.qrc
