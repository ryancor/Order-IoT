# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)
	# to pick the right linux-headers, type in cmd `uname -a`
	KERNEL_SOURCE := /usr/src/linux-headers-4.13.0-41-generic
	PWD := $(shell pwd)
default: module query_app

module:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
	${RM} query_app

else
	obj-m := query_ioctl.o
endif
