obj-m+=process_list.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) user_process_list.c -o process_list

insmod:
	sudo insmod process_list.ko

run:
	sudo ./process_list

rmmod:
	sudo rmmod process_list

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm process_list
