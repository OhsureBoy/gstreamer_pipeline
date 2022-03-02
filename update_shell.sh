#!/bin/bash
PID=`ps -eaf | grep frontkey.py | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  echo "nvidia" | sudo -S kill -9 $PID
fi

PID=`ps -eaf | grep streaming_state.py | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  echo "nvidia" | sudo -S kill -9 $PID
fi

PID=`ps -eaf | grep tlc_bond | grep -v grep | awk '{print $2}'`
echo "$PID"
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  echo "nvidia" | sudo -S kill -9 $PID
fi

#파일을 덮어 씌우는 부분 &&는 성공하면 다음 명령어 실행

chmod 777 ./update/frontkey.py 
chmod 777 ./update/tlc_bond 
chmod 777 ./update/streaming_state.py 
chmod 777 ./update/update_shell.sh

mv -f ./update/frontkey.py ./frontkey.py  &&
mv -f ./update/tlc_bond ./tlc_bond &&
mv -f ./update/streaming_state.py ./streaming_state.py &&
mv -f ./update/libgstfaac.so /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstfaac.so &&
mv -f ./update/libgstmcts.so /usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstmcts.so &&
mv -f ./update/update_shell.sh ./update_shell.sh &&

# mv ..path /gstreamer lib

rm -rf ./update/TLC_BOND.zip

#프로그램을 실행하는 부분.
./frontkey.py &
./streaming_state.py -t1  &
./tlc_bond -m &
