#!/bin/sh
if [ $# -ne 0 ]
then
    INSTALL_PREFIX="$1"
else
    INSTALL_PREFIX="/usr"
fi
./setup.sh
sudo mkdir -p /opt/npdfr
sudo cp -v -r bin tools venv /opt/npdfr
sudo cp -v sys/run_installed.sh "$INSTALL_PREFIX/bin/npdfr"
sudo rsync -av sys/share/ "$INSTALL_PREFIX/share/"
