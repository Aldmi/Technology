
Сборка и запуск
---------------------------------------------------------------------------------------------------
1. Build GidUralClient
cd /root/gid/test/giduralclient && sudo make build

2.run
Запуск sudo (в начале войти в систему как root)
sudo -s 
source /root/gid/GidUral_CICD/.env && cd /root/gid/test/giduralclient/build && ./GidUralClient


tmux:

 tmux new -s gidUral
 tmux attach -t gidUral
 tmux kill-session -t gidUral

detach session:
ctrl+b, d




Локальный запуск /home/aldmi/CLionProjects/GidUralClient/cmake-build-debug/
sudo -s 
source ./env.txt && ./GidUralClient

docker exec -it b5d116ad83cc bash












