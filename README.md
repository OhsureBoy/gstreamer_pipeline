# gstreamer_pipeline


Nvidia Jetson Nano
plugin faac

gst-inspect로 검색해서 하는 방법과, 인터넷으로 API 문서를 보고 하는 방법이 있다





//jpg
gst-launch-1.0 -e --gst-debug=**:4 v4l2src device=/dev/video0 \
! "video/x-raw,format=(string)UYVY, width=1920, height=1080" \
! videoscale \
! "video/x-raw, width=80,height=60" \
! videorate \
! "video/x-raw, framerate=5/1" \
! identity sync=true \
! timeoverlay \
! jpegenc \
! multifilesink location="file.jpg"




//tee
gst-launch-1.0 -e --gst-debug=**:4 v4l2src device=/dev/video0 \
! "video/x-raw,format=(string)UYVY, width=1920, height=1080" \
! tee name = t \
t. ! queue \
! nvvidconv \
! "video/x-raw(memory:NVMM),format=(string)I420" \
! nvv4l2h264enc  bitrate=40000000 insert-sps-pps=true  \
! h264parse \
! muxer.video_0 \
mp4mux name=muxer  \
!  filesink location="mystream.mp4" sync=false \
t. ! queue \
! videoscale \
! "video/x-raw, width=80,height=60" \
! videorate \
! "video/x-raw, framerate=5/1" \
! identity sync=true \
! timeoverlay \
! jpegenc \
! filesink location="file.jpg"


//UVC Camera

gst-launch-1.0 v4l2src device=/dev/video0 io-mode=2 \
! image/jpeg,width=1920,height=1080 \
! nvjpegdec \
! video/x-raw \
!queue \
! nvvidconv \
! 'video/x-raw(memory:NVMM),format=I420' \
!queue \
! nvv4l2h264enc \
! h264parse \
!queue \
! mpegtsmux \
! filesink location="aa.mp4"
