#！/bin/bash
gst-launch-1.0 videotestsrc is-live=true ! video/x-raw,format=I420,framerate=15/1 ! \
x264enc aud=false bframes=0 speed-preset=veryfast key-int-max=15 ! video/x-h264,stream-format=byte-stream,profile=baseline ! \
h264parse ! rtph264pay config-interval=-1  pt=96 ! udpsink host=127.0.0.1 port=5000