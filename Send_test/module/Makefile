obj-m += hello_kernel_mod.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
load:
	sudo insmod hello_kernel_mod.ko
	
unload:
	sudo rmmod hello_kernel_mod.ko
