#!/bin/bash
# Camera shooting shell script
DATE=$(date +"%Y-%m-%d_%H%M")

 raspistill -t 500 -o /home/pi/camera/$DATE.jpg
