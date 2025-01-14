#!/bin/bash

#camera for /dev/video0
gst-launch-1.0 v4l2src io-mode=0 device=/dev/video0 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! xvimagesink sync=false &
sleep 2

#camera for /dev/video1  port number will be changed depending on the connection of other cameras
gst-launch-1.0 v4l2src io-mode=0 device=/dev/video1 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! xvimagesink sync=false &
sleep 2

#camera for /dev/video2  port number will be changed depending on the connection of other cameras
gst-launch-1.0 v4l2src io-mode=0 device=/dev/video2 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! xvimagesink sync=false &
sleep 2

#camera for /dev/video3  port number will be changed depending on the connection of other cameras
gst-launch-1.0 v4l2src io-mode=0 device=/dev/video3 do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! xvimagesink sync=false &
sleep 2
