#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T14:09:30
#
#-------------------------------------------------

TARGET = GDeskTunes
TEMPLATE = app

QT += core gui webkit widgets network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

# Needed to deploy the userstyles and java scripts to the correct directory
CONFIG(release, release|debug): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

# WebEngine implementation
#
# QT += webengine webenginewidgets
CONFIG += c++11

win32: {

    greaterThan(QT_MAJOR_VERSION, 4): greaterThan(QT_MINOR_VERSION, 0): QT += winextras

    # Resource file for windows systems
    RC_FILE = ../gdesktunes.rc

    LIBS += -luser32 -lshell32
}

mac: {

    QMAKE_INFO_PLIST = ../Info.plist

    ICON = ../g_desk_tunes.icns

    OBJECTIVE_SOURCES += \
        mac/cocoainit.mm \
        mac/mediakeys.mm \
        mac/notificationcenter.mm

    HEADERS += \
        mac/cocoainit.h \
        mac/mediakeys.h \
        mac/notificationcenter.h

    QMAKE_OBJECTIVE_CFLAGS_WARN_ON = -Wall -Wno-unused-parameter -Wno-deprecated-declarations -Wno-incomplete-implementation -Wno-receiver-forward-class

    LIBS += -framework Cocoa -framework Carbon -framework CoreFoundation -framework SystemConfiguration

    greaterThan(QT_MAJOR_VERSION, 4): QMAKE_MAC_SDK = macosx10.9

    lessThan(QT_MAJOR_VERSION, 6): {

        info.path = $$DESTDIR/$${TARGET}.app/Contents
        info.files = $$QMAKE_INFO_PLIST
        export(info)

        icns.path = $$DESTDIR/$${TARGET}.app/Contents/Resources
        icns.files = $$ICON
        export(icns)

        INSTALLS += icns info
    }
}

# Bonjour dependencies
!mac:x11:LIBS+=-ldns_sd

win32 {
    # Add your path to bonjour here.
    INCLUDEPATH += "C:/Program Files/Bonjour SDK/Include"

    contains(QMAKE_HOST.arch, x86_64){
        LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/x64" -ldnssd
    } else {
        LIBS+= -L"C:/Program Files/Bonjour SDK/Lib/Win32" -ldnssd
    }
}


INCLUDEPATH += ..

HEADERS += \
    aboutdialog.h \
    cookiejar.h \
    gdesktunes.h \
    googlemusicapp.h \
    lastfm.h \
    mainwindow.h \
    settings.h \
    networkmanager.h \
    systemtrayicon.h \
    miniplayer.h \
    artlabel.h \
    application.h \
    qutils.h \
    statemachine.h \
    slider.h \
    downloader.h \
    webview.h \
    mediakey.h \
    versioncheck.h \
    server/remoteserver.h \
    mediakeydialog.h \
    gdeskinterface.h

SOURCES += \
    aboutdialog.cpp \
    cookiejar.cpp \
    gdesktunes.cpp \
    googlemusicapp.cpp \
    lastfm.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    networkmanager.cpp \
    systemtrayicon.cpp \
    miniplayer.cpp \
    artlabel.cpp \
    application.cpp \
    qutils.cpp \
    statemachine.cpp \
    slider.cpp \
    downloader.cpp \
    webview.cpp \
    versioncheck.cpp \
    server/remoteserver.cpp \
    mediakey.cpp \
    mediakeydialog.cpp \
    gdeskinterface.cpp

RESOURCES += \
    ../gdesktunes.qrc

FORMS += \
    mainwindow.ui \
    settings.ui \
    aboutdialog.ui \
    miniplayer.ui \
    mediakeydialog.ui

win32|linux: css.path = $$DESTDIR/userstyles
mac: css.path = $$DESTDIR/GDeskTunes.app/Contents/Resources/userstyles
css.files = ../userstyles/*.css
css.files += ../userstyles/*.scss
export(css)

win32|linux: minicss.path = $$DESTDIR/userstyles/mini
mac: minicss.path = $$DESTDIR/GDeskTunes.app/Contents/Resources/userstyles/mini
minicss.files = ../userstyles/mini/*
export(minicss)

win32|linux: material.path = $$DESTDIR/userstyles
mac: material.path = $$DESTDIR/GDeskTunes.app/Contents/Resources/userstyles
material.files = ../userstyles/Material/*
export(material)

win32|linux: js.path = $$DESTDIR/js
mac: js.path = $$DESTDIR/GDeskTunes.app/Contents/Resources/js
js.files = ../js/*
export(js)

win32: {
    gdeskhook.path = $$DESTDIR
    contains(QMAKE_HOST.arch, x86_64): {
        gdeskhook.files = ../bin/GDeskHook.exe ../bin/MMSHellHook.dll
    } else {
        gdeskhook.files = ../bin/GDeskHook64.exe ../bin/MMSHellHook64.dll
    }
    export(gdeskhook)
    INSTALLS += gdeskhook
}

INSTALLS += css minicss material js

# Liblastfm
DEFINES += LASTFM_LIB_STATIC

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/liblastfm/release/ -lliblastfm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/liblastfm/debug/ -lliblastfm
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/liblastfm/ -lliblastfm

INCLUDEPATH += $$PWD/../thirdparty/liblastfm
DEPENDPATH += $$PWD/../thirdparty/liblastfm

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/liblastfm/release/libliblastfm.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/liblastfm/debug/libliblastfm.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/liblastfm/release/liblastfm.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/liblastfm/debug/liblastfm.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/liblastfm/libliblastfm.a

# SPMediaKeyTap
mac: {
    LIBS += -L$$OUT_PWD/../thirdparty/SPMediaKeyTap/ -lSPMediaKeyTap

    INCLUDEPATH += $$PWD/../thirdparty/SPMediaKeyTap
    DEPENDPATH += $$PWD/../thirdparty/SPMediaKeyTap

    PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/SPMediaKeyTap/libSPMediaKeyTap.a
}

# qt-solutions
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/qt-solutions/release/ -lqt-solutions
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/qt-solutions/debug/ -lqt-solutions
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/qt-solutions/ -lqt-solutions

INCLUDEPATH += $$PWD/../thirdparty/qt-solutions
DEPENDPATH += $$PWD/../thirdparty/qt-solutions

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/qt-solutions/release/libqt-solutions.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/qt-solutions/debug/libqt-solutions.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/qt-solutions/release/qt-solutions.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/qt-solutions/debug/qt-solutions.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/qt-solutions/libqt-solutions.a

# QtWaitingSpinner
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/QtWaitingSpinner/release/ -lQtWaitingSpinner
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/QtWaitingSpinner/debug/ -lQtWaitingSpinner
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/QtWaitingSpinner/ -lQtWaitingSpinner

INCLUDEPATH += $$PWD/../thirdparty/QtWaitingSpinner
DEPENDPATH += $$PWD/../thirdparty/QtWaitingSpinner

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/QtWaitingSpinner/release/libQtWaitingSpinner.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/QtWaitingSpinner/debug/libQtWaitingSpinner.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/QtWaitingSpinner/release/QtWaitingSpinner.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/QtWaitingSpinner/debug/QtWaitingSpinner.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/QtWaitingSpinner/libQtWaitingSpinner.a

# Bonjourgear
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/bonjourgear/release/ -lbonjourgear
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/bonjourgear/debug/ -lbonjourgear
else:unix: LIBS += -L$$OUT_PWD/../lib/bonjourgear/ -lbonjourgear

INCLUDEPATH += $$PWD/../lib/bonjourgear
DEPENDPATH += $$PWD/../lib/bonjourgear

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/bonjourgear/release/libbonjourgear.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/bonjourgear/debug/libbonjourgear.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/bonjourgear/release/bonjourgear.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/bonjourgear/debug/bonjourgear.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/bonjourgear/libbonjourgear.a

# MMShellHook
contains(QMAKE_HOST.arch, x86_64){
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/MMShellHook/release/ -lMMShellHook64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/MMShellHook/debug/ -lMMShellHook64
} else {
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/MMShellHook/release/ -lMMShellHook
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/MMShellHook/debug/ -lMMShellHook
}
INCLUDEPATH += $$PWD/../lib/MMShellHook
DEPENDPATH += $$PWD/../lib/MMShellHook

# GDeskStyler
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../tools/GDeskStyler/release/ -lGDeskStyler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../tools/GDeskStyler/debug/ -lGDeskStyler
else:unix: LIBS += -L$$OUT_PWD/../tools/GDeskStyler/ -lGDeskStyler

INCLUDEPATH += $$PWD/../tools/GDeskStyler
DEPENDPATH += $$PWD/../tools/GDeskStyler

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../tools/GDeskStyler/release/libGDeskStyler.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../tools/GDeskStyler/debug/libGDeskStyler.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../tools/GDeskStyler/release/GDeskStyler.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../tools/GDeskStyler/debug/GDeskStyler.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../tools/GDeskStyler/libGDeskStyler.a

# Libsass
macx: LIBS += -L$$PWD/../thirdparty/libsass/lib/ -lsass
win32: LIBS += -L$$PWD/../thirdparty/libsass/win/bin/ -llibsass

INCLUDEPATH += $$PWD/../thirdparty/libsass
DEPENDPATH += $$PWD/../thirdparty/libsass

macx: PRE_TARGETDEPS += $$PWD/../thirdparty/libsass/lib/libsass.a

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../thirdparty/libsass/win/bin/libsass.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../thirdparty/libsass/win/bin/liblibsass.a
