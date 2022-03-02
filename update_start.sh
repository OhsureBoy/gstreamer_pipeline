#!/bin/bash
#파일을 여는 부분
unzip ./update/TLC_BOND.zip -d ./update/
chmod 777 ./update/update_shell.sh

./update/update_shell.sh
