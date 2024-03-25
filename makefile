all: producer consumer main

producer: producer.c
	gcc -o producer producer.c -Wall -Werror

consumer: consumer.c distributor.c put.c get.c worker.c
	gcc -o consumer consumer.c distributor.c put.c get.c worker.c -lpthread -Wall -Werror 

main: main.c 
	gcc -o sync main.c -lpthread -Wall -Werror 