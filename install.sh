#!/usr/bin/bash

# copy service file to /etc/systemd/system
sudo cp ./startEngineer.service /etc/systemd/system

# copy .rule file to /etc/udev/rules.d
sudo cp usb.rules /etc/udev/rules.d

echo "reboot to take effect"