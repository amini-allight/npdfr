#!/bin/sh
python -m venv ./venv
source ./venv/bin/activate
pip install -r requirements.txt
mkdir -p build
cd build
cmake ..
make -j$(nproc)
