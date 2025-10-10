include(./common_include.pri)

TEMPLATE = subdirs
SUBDIRS = \
    server \
    rofi

OTHER_FILES += qlipmon.xml arch/PKGBUILD
VERSION = 0.2.0