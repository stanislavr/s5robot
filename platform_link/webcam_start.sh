#!/bin/bash

echo "Starting webcam"
mjpg_streamer -i "/usr/lib/input_uvc.so -y" -o "/usr/lib/output_http.so -w ./www"
