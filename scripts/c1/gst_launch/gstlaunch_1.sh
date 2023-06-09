#!/bin/bash

# Check arg
if [ $# -ne 1 ];
then
  echo "Usage: ./gstlaunch_single_cam.sh <devfile> (ex. /dev/video0)"
  exit 1
fi


set -x
gst-launch-1.0 v4l2src io-mode=0 device=$1 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! videoscale ! xvimagesink sync=false
