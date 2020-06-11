#!/bin/sh -ex

cd `dirname "$0"`;

#PROCESSOR="qdbusxml2cpp -V -N"
PROCESSOR="qdbusxml2cpp -V"

function process_file(){
	sed 's/\\"DOMAIN.DOMAIN\\"/\\"" QLIPMON_DBUS_FQDN "\\"/g' -i $1
	sed 's/\"DOMAIN.DOMAIN\"/QLIPMON_DBUS_FQDN/g' -i $1
	sed 's/DOMAIN.DOMAIN/QLIPMON_DBUS_FQDN/g' -i $1
}

cd server

$PROCESSOR -c QlipmonAdaptor -i database_entry.h -a qlipmon_adaptor.h: ../qlipmon.xml
$PROCESSOR -c QlipmonAdaptor -i qlipmon_adaptor.h -a :qlipmon_adaptor.cpp ../qlipmon.xml

process_file qlipmon_adaptor.h;

cd ../rofi
$PROCESSOR -i ../server/database_entry.h --classname QlipMonInterface -p qlipmon_interface.h: ../qlipmon.xml
$PROCESSOR -i qlipmon_interface.h --classname QlipMonInterface -p :qlipmon_interface.cpp ../qlipmon.xml

process_file qlipmon_interface.h
