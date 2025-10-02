# Common configuration for QlipMon project
# This file sets up shared include paths, dependencies, and build configuration

# Get the absolute path to the project root
PROJECT_ROOT = $$PWD

# Add common include directory to include path
INCLUDEPATH += $$PROJECT_ROOT/common

# Define common headers directory
COMMON_HEADERS_DIR = $$PROJECT_ROOT/common

# Version information
VERSION = "0.2.0"
APP_NAME = "Qlipmon"

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_NAME=\\\"$$APP_NAME\\\"

# DBus configuration
isEmpty(QLIPMON_DBUS_FQDN){
	QLIPMON_DBUS_FQDN = org.qlipmon
}

isEmpty(QLIPMON_DBUS_PATH){
	QLIPMON_DBUS_PATH = /Qlipmon
}

isEmpty(PREFIX){
	PREFIX = /usr
}

DEFINES += QLIPMON_DBUS_FQDN='\'"$$QLIPMON_DBUS_FQDN"\''
DEFINES += QLIPMON_DBUS_PATH='\'"$$QLIPMON_DBUS_PATH"\''

# Disable qDebug() in release builds
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Messages for debugging
!build_pass:message("Common include path: $$COMMON_HEADERS_DIR")
!build_pass:message("DBUS FQDN: $$QLIPMON_DBUS_FQDN")
!build_pass:message("DBUS PATH: $$QLIPMON_DBUS_PATH")