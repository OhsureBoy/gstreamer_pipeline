#!/bin/bash
PID=`ps -eaf | grep frontkey.py | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  kill -9 $PID
fi

PID=`ps -eaf | grep streaming_state.py | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  kill -9 $PID
fi

PID=`ps -eaf | grep receiver | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  kill -9 $PID
fi

#파일을 여는 부분
unzip /home/nvidia/web/TLC_BOND.zip

#파일을 덮어 씌우는 부분 &&는 성공하면 다음 명령어 실행

chmod 777 frontkey.py
chmod 777 receiver
chmod 777 streaming_state.py

mv /home/nvidia/web/frontkey.py /home/nvidia/display/tbond/frontkey.py  &&
mv /home/nvidia/web/receiver /home/nvidia/display/tbond/receiver &&
mv /home/nvidia/web/streaming_state.py /home/nvidia/display/tbond/streaming_state.py &&
# mv ..path /gstreamer lib

rm -rf TLC_BOND.zip

#프로그램을 실행하는 부분.
/home/nvidia/display/tbond/frontkey.py &
/home/nvidia/display/tbond/receiver &
/home/nvidia/display/tbond/streaming_state.py -t1 "title: 홍길동 라이브" -t2 "SRC None" -t3 "DST 1080P 30fps H.265" -t3 "BPS 3000kbps" -t4 "Not Connected"
