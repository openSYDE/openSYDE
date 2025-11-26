build:
	cd bat
	./build.sh -t rpi64

copy:
	scp result/raspberrypi64/SYDEsup init@10.149.49.131:~/

# run on remote
run:
	ssh init@10.149.49.131
# 	chmod +x ~/SYDEsup
# 	./SYDEsup -h