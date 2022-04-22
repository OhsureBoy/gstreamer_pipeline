# gstreamer_pipeline


Nvidia Jetson Nano plugin faac   <br>
cUrl, Json C Language <br>

gst-inspect로 검색해서 하는 방법과, 인터넷으로 API 문서를 보고 하는 방법이 있다. 기억용으로 남기기 <br>

<br>
<br>

GPIO / cJASON / C / python / python /  REST API / cURL /gstreamer / SPI 통신<br>
아래 파이프 라인은 receiver.c 에 있다

<br>
Gstreamer 수정중
<br>
<br>


//jpg gst-launch-1.0 -e --gst-debug=**:4 v4l2src device=/dev/video0  <br>
! "video/x-raw,format=(string)UYVY, width=1920, height=1080"  <br>
! videoscale  <br>
! "video/x-raw, width=80,height=60"  <br>
! videorate  <br>
! "video/x-raw, framerate=5/1"  <br>
! identity sync=true  <br>
! timeoverlay  <br>
! jpegenc  <br>
! multifilesink location="file.jpg"  <br>
  <br>
//tee gst-launch-1.0 -e --gst-debug=**:4 v4l2src device=/dev/video0  <br>
! "video/x-raw,format=(string)UYVY, width=1920, height=1080"  <br>
! tee name = t  <br>
t. ! queue  <br>
! nvvidconv  <br>
! "video/x-raw(memory:NVMM),format=(string)I420"  <br>
! nvv4l2h264enc bitrate=40000000 insert-sps-pps=true  <br>
! h264parse  <br>
! muxer.video_0  <br>
mp4mux name=muxer  <br>
! filesink location="mystream.mp4" sync=false  <br>
t. ! queue  <br>
! videoscale  <br>
! "video/x-raw, width=80,height=60"  <br>
! videorate  <br>
! "video/x-raw, framerate=5/1"  <br>
! identity sync=true  <br>
! timeoverlay  <br>
! jpegenc  <br>
! filesink location="file.jpg"  <br>
  <br>
//UVC Camera
  <br>
gst-launch-1.0 v4l2src device=/dev/video0 io-mode=2  <br>
! image/jpeg,width=1920,height=1080  <br>
! nvjpegdec  <br>
! video/x-raw  <br>
!queue  <br>
! nvvidconv  <br>
! 'video/x-raw(memory:NVMM),format=I420'  <br>
!queue  <br>
! nvv4l2h264enc  <br>
! h264parse  <br>
!queue  <br>
! mpegtsmux  <br>
! filesink location="aa.mp4"  <br>
