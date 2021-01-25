.PHONY:all clean
WORK_ROOT=../../../
KERNEL_ROOT=${WORK_ROOT}/linux_4.0
ifneq ($(KERNELRELEASE),)

obj-m := ${OBJ}.o
${OBJ}-objs := main.o super.o samplefs_debug.o

else

EXTRA_CFLAGS += -DDEBUG
EXTRA_CFLAGS += -DCONFIG_PROC_FS
KDIR := $(KERNEL_ROOT)
all:
ifeq ("$(origin OBJ)", "undefined")
		@echo "please input make OBJ=xxx , xxx is object"
else
		make  CROSS_COMPILE=arm-linux-gnueabi- ARCH=arm -C $(KDIR) M=$(PWD) modules && cp *.ko ${WORK_ROOT}/busybox/rootfs/work
endif
clean:
		rm -fr *.ko *.o *.mod.o *.mod.c *.symvers *.order .*.ko .tmp_versions .*.cmd

endif
