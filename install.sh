#!/bin/sh
#build the program and copy it to proper folder
#gcc mult_adaptor.cpp -o card_reader_adaptor_bin -lwiringPi -lcurl
sudo make
sudo ps aux |grep /usr/bin/stm32_client_bin | awk '{print $2}'|xargs kill -9

if [ ! -d "/home/pi/log/" ]; then
	mkdir -p /home/pi/log/stm32_client/
fi
PID_FILE=/var/run/supervisord.pid

PID=$(cat "${PID_FILE}")
sudo kill -9 ${PID}
#rm /home/pi/log/stm32_client/*
sudo supervisorctl stop stm32_client
sudo supervisorctl remove stm32_client
if [ -f "/etc/supervisor/conf.d/stm32_client.conf" ] ; then
	sudo rm /etc/supervisor/conf.d/stm32_client.conf
fi
if [ -f "/etc/supervisor/conf.d/stm32_clent.conf" ] ; then
	sudo rm /etc/supervisor/conf.d/stm32_clent.conf
fi 
sudo pkill stm32
if [ -f "/home/pi/log/stm32_client/voltage.log" ] ; then
	sudo mv /home/pi/log/stm32_client/voltage.log /home/pi/log/stm32_client/voltage.log.backup
fi
if [ -f "/home/pi/log/stm32_client/temperature.log" ] ; then 
	sudo mv /home/pi/log/stm32_client/temperature.log /home/pi/log/stm32_client/temperature.log.backup
fi
sudo chown root:root stm32_client_bin
sudo cp stm32_client_bin /usr/bin/
sudo chmod +x /usr/bin/stm32_client_bin

#sudo rm *.cpp
#sudo rm *.c
#sudo rm *.h
#sudo rm -rf .git

sudo cp stm32_client.conf /etc/supervisor/conf.d/ 
sudo /etc/init.d/supervisor restart 
