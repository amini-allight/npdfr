#!/bin/sh
./setup.sh
sudo mkdir -p /opt/npdfr
sudo cp -r bin tools venv /opt/npdfr
sudo cp sys/run_installed.sh /usr/bin/npdfr
