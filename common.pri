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

#disable qDebug() in release builds
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
