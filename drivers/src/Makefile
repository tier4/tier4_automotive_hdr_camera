HOST_ARCH       ?= $(shell uname -m | sed -e s/arm.*/arm/ -e s/aarch64.*/arm64/)
ARCH            ?= $(shell uname -m | sed -e s/arm.*/arm/ -e s/aarch64.*/arm64/)
PWD 			      ?= ${shell pwd}
BUILD_DIR       ?= $(shell pwd)
_BUILD_DIR      ?= $(shell readlink -f $(BUILD_DIR))

INST_PACKAGE_NAME    ?=  $(shell cat dkms.conf | grep 'PACKAGE_NAME' | cut -d '=' -f 2 | cut -d '"' -f 2 )
INST_PACKAGE_VERSION ?=  $(shell cat dkms.conf | grep 'PACKAGE_VERSION' | cut -d '=' -f 2 | cut -d '"' -f 2 )

TIER4_ISX021_FIRMWARE_BINARY := tier4-isx021.bin

DEST_FIRMWARE_DIR    := /lib/firmware
SRC_FIRMWARE_BINARY  := /usr/src/$(INST_PACKAGE_NAME)-$(INST_PACKAGE_VERSION)/$(TIER4_ISX021_FIRMWARE_BINARY)
DEST_FIRMWARE_BINARY := $(DEST_FIRMWARE_DIR)/$(TIER4_ISX021_FIRMWARE_BINARY)

ifdef KERNELRELEASE
	KERNELRELEASE := $(KERNELRELEASE)
else
	KERNELRELEASE ?= $(shell uname -r)
endif

ifdef KERNEL_SRC
  KERNEL_SRC_DIR  := $(KERNEL_SRC)
else
  KERNEL_SRC_DIR  ?= /lib/modules/$(shell uname -r)/build
endif

ifeq ($(ARCH), arm)
 ifneq ($(HOST_ARCH), arm)
   CROSS_COMPILE  ?= arm-linux-gnueabihf-
 endif
endif
ifeq ($(ARCH), arm64)
 ifneq ($(HOST_ARCH), arm64)
   CROSS_COMPILE  ?= aarch64-linux-gnu-
 endif
endif

ccflags-y += -DBUILD_STAMP="\"`date +'%Y-%m-%d %H:%M:%S'`\""

obj-m := tier4-max9295.o tier4-max9296.o tier4-isx021.o

.PHONY: all
all: binary_module modules dtbo

modules:
	$(MAKE) -C $(KERNEL_SRC_DIR) ARCH=$(ARCH) M=$(BUILD_DIR) src=$(PWD) modules
binary_module:
	install -d $(DEST_FIRMWARE_DIR)
	cp -f $(SRC_FIRMWARE_BINARY) $(DEST_FIRMWARE_BINARY)
dtbo:
	dtc -O dtb -o tier4-isx021-gmsl-device-tree-overlay.dtbo -@ tier4-isx021-gmsl-device-tree-overlay.dts
	cp -rf tier4-isx021-gmsl-device-tree-overlay.dtbo /boot
clean:
	$(MAKE) -C $(KERNEL_SRC_DIR) ARCH=$(ARCH) M=$(BUILD_DIR) src=$(PWD) clean
	# rm /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo
