c: socket
	gcc -o chatroom -Wall -O0 -ggdb -Wl,-rpath=/home/twngbm/C socket_thread.c  -L. -lSocket -pthread
socket:
	gcc -c -Wall -Werror -O0 -ggdb -fpic Socket.c
	gcc -shared -o libSocket.so Socket.o