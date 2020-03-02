#!/bin/bash
gst-launch-1.0 -vv -e v4l2src device=$1 \
! "video/x-raw,width=1920, height=1080, format=(string)YUY2", framerate=30/1 \
! videoconvert \
! x264enc pass=qual quantizer=5 tune=zerolatency \
! h264parse \
! rtph264pay config-interval=10 pt=96 \
! udpsink host=$2 port=$3
