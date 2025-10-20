ARCH          = arm64
KDIR          = /lib/modules/$(shell uname -r)/build/
CROSS_COMPILE =

ccflags-y += -DBUILD_STAMP="\"`date +'%Y-%m-%d %H:%M:%S'`\""

obj-m := max20089.o tier4-fpga.o tier4-gw5300.o tier4-imx490.o tier4-imx728.o tier4-isx021.o tier4-max9295.o tier4-max9296.o

all: modules

modules:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean

load: modules unload
	sudo insmod max20089.ko
	sudo insmod tier4-fpga.ko
	sudo insmod tier4-max9295.ko
	sudo insmod tier4-max9296.ko
	sudo insmod tier4-gw5300.ko
	sudo insmod tier4-isx021.ko
	sudo insmod tier4-imx490.ko
	sudo insmod tier4-imx728.ko

unload:
	sudo rmmod tier4-imx728 || true
	sudo rmmod tier4-imx490 || true
	sudo rmmod tier4-isx021 || true
	sudo rmmod tier4-gw5300 || true
	sudo rmmod tier4-max9296 || true
	sudo rmmod tier4-max9295 || true
	sudo rmmod tier4-fpga || true
	sudo rmmod max20089 || true

.PHONY: all modules clean load unload
