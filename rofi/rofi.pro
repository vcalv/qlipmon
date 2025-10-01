include(../common.pri)
include(../common_include.pri)

TARGET = qlipmon

QT = dbus

TEMPLATE = lib
DEFINES += ROFIQLIPMON_LIBRARY

CONFIG += plugin
CONFIG += no_plugin_name_prefix
CONFIG += skip_target_version_ext

#CONFIG += c++17 link_pkgconfig
#PKGCONFIG += rofi

CONFIG += c++17

#ugly hack because qmake replaces -I/usb/include by -isystem /usr/include and breaks g++
QMAKE_CXXFLAGS += $$system("pkgconf --cflags-only-I rofi")

#I just need the headers, no need to link agains QtGUI
QMAKE_CXXFLAGS += $$system("pkgconf --cflags-only-I Qt6Gui")

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    qlipmon_interface.cpp \
    config.cpp \
    qlipdata.cpp \
    rofiqlipmon.cpp \
    ../server/database_entry.cpp \

HEADERS += \
    qlipmon_interface.h \
    config.h \
    qlipdata.h \
    rofi-qlipmon_global.h \

# Default rules for deployment.
unix {
    target.path = $${PREFIX}/lib/rofi/
}
!isEmpty(target.path): INSTALLS += target
