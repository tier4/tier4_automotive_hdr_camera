#!/bin/bash

help()
{
    echo "Record streaming video in .mp4" 
    echo
    echo "Syntax: gstlaunhc_record_1.sh [-b|d|o|h]"
    echo "Options:"
    echo "b     Set bitrate (Default: 3000000)"
    echo "d     Specify video device (Default: /dev/video0)"
    echo "o     Specify output file path (Default: out.mp4)"
    echo "h     Print this Help."
    echo
}

enc_bitrate=3000000
video_dev=/dev/video0
output_path=out.mp4

while getopts hb:d:o: option; do
    case $option in
        h) # Display help
            help
            exit;;
        b) # Get bitrate for encoding
            enc_bitrate=$OPTARG;;
        d) # Get video device
            video_dev=$OPTARG;;
        o) # Get output path
            output_path=$OPTARG;;
        \?) # Invalid option
            echo "Error: Invalid option"
            exit;;
    esac
done



set -x
gst-launch-1.0 -e v4l2src io-mode=0 device=${video_dev} do-timestamp=true ! 'video/x-raw, width=1920, height=1280, framerate=30/1, format=UYVY' ! nvvidconv ! nvv4l2h265enc bitrate=${enc_bitrate} ! h265parse !  qtmux ! filesink location=${output_path}
