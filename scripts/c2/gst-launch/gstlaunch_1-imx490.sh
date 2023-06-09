#!/bin/bash

# Check arg

set -x
gst-launch-1.0 v4l2src io-mode=0 device=/dev/video0 do-timestamp=true ! 'video/x-raw, width=2880, height=1860, framerate=30/1, format=UYVY' ! videoscale ! xvimagesink sync=false
