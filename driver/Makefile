# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)
	# to pick the right linux-headers, type in cmd `uname -r`
	KERNEL_SOURCE := /usr/src/linux-headers-$(shell uname -r)
	PWD := $(shell pwd)
default: module query_app kernel_app

module:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
	${RM} query_app kernel_app

else
	obj-m := query_ioctl.o kernel_bus.o
endif
