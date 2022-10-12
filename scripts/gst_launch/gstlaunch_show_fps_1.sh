#!/bin/bash

# Check arg
if [ -e /dev/gmsl/tier4-isx021-cam2 ];
then
  gst-launch-1.0 v4l2src io-mode=0 device=/dev/video0 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! xvimagesink sync=false \
  v4l2src io-mode=0 device=/dev/video1 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! fpsdisplaysink video-sink=xvimagesink sync=false
else
  gst-launch-1.0 v4l2src io-mode=0 device=/dev/video0 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! fpsdisplaysink video-sink=xvimagesink sync=false
fi
