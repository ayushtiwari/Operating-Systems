producer: shared.c queue.c producer.c
	gcc shared.c queue.c producer.c -o producer

consumer: shared.c queue.c consumer.c
	gcc shared.c queue.c consumer.c -o consumer

a.out: shared.c queue.c main.c producer consumer
	gcc shared.c queue.c main.c -o a.out

clean:
	rm a.out
	rm consumer 
	rm producer
