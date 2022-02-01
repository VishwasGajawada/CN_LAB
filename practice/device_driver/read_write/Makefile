name 	:= read_write
obj-m	:= $(name).o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
insert:
	sudo insmod $(name).ko
remove:
	sudo rmmod $(name).ko
log:
	dmesg | tail
