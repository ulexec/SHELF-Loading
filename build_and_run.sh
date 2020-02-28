docker build -t shelf . &
sleep 15
sudo docker cp $(sudo docker ps -l | awk -F 'CONTAINER' '{print $1}' | awk -F ' ' '{print $1}'):/SHELF/loader .
