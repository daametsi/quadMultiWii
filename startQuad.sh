#!/bin/sh
# Script to start quad control program
cd /home/ubuntu/quad/control
make || exit 1
/home/ubuntu/quad/control/setevn.sh
/home/ubuntu/quad/control/control &
