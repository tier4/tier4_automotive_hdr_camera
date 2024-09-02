HOST_ARCH       ?= $(shell uname -m | sed -e s/arm.*/arm/ -e s/aarch64.*/arm64/)
ARCH            ?= $(shell uname -m | sed -e s/arm.*/arm/ -e s/aarch64.*/arm64/)
PWD             ?= ${shell pwd}
BUILD_DIR       ?= $(shell pwd)
_BUILD_DIR      ?= $(shell readlink -f $(BUILD_DIR))

Xavier_word           := Xavier
Xavier_other_word_1   := Jetson-AGX
Orin_word             := Orin
Orin_Compatible_word  := p3737-0000+p3701-0000nvidia
ROScube_word          := ROScube-X
ROScube_Orin_word     := 590
ROScube_Orin_Model_word  := ROScube-Orin
EAC_5000_word         := EAC-5000
Anvil_word            := CTI Forge + Orin AGX
Anvil_Model_word      := Anvil


INST_PACKAGE_NAME     ?=  $(shell cat dkms.conf | grep 'PACKAGE_NAME' | cut -d '=' -f 2 | cut -d '"' -f 2 )
INST_PACKAGE_VERSION  ?=  $(shell cat dkms.conf | grep 'PACKAGE_VERSION' | cut -d '=' -f 2 | cut -d '"' -f 2 )

TIER4_ISX021_FIRMWARE_BINARY := tier4-isx021.bin

DEST_FIRMWARE_DIR    := /lib/firmware
SRC_FIRMWARE_BINARY  := /usr/src/$(INST_PACKAGE_NAME)-$(INST_PACKAGE_VERSION)/$(TIER4_ISX021_FIRMWARE_BINARY)
DEST_FIRMWARE_BINARY := $(DEST_FIRMWARE_DIR)/$(TIER4_ISX021_FIRMWARE_BINARY)

# KERNELRELEASE ?= $(shell uname -r)

dt_model := $(shell cat /proc/device-tree/model | tr -d "\0" )

dt_compatible := $(shell cat /proc/device-tree/compatible | tr -d "\0")

L4T_RELEASE_MAIN  := $(shell head -n 1 /etc/nv_tegra_release | cut -d ' ' -f 2 )
L4T_RELEASE_MAJOR := $(shell head -n 1 /etc/nv_tegra_release | cut -d ' ' -f 5 | cut -d ',' -f 1 | cut -d '.' -f 1 )
L4T_RELEASE_MINOR := $(shell head -n 1 /etc/nv_tegra_release | cut -d ' ' -f 5 | cut -d ',' -f 1 | cut -d '.' -f 2 )
L4T_RELEASE       := $(L4T_RELEASE_MAIN).$(L4T_RELEASE_MAJOR).$(L4T_RELEASE_MINOR)

$(warning L4T_RELEASE_MAIN  = $(L4T_RELEASE_MAIN))
$(warning L4T_RELEASE_MAJOR = $(L4T_RELEASE_MAJOR))
$(warning L4T_RELEASE_MINOR = $(L4T_RELEASE_MINOR))

$(warning dt_compatible = $(dt_compatible))
$(warning Orin_Compatible_word = $(Orin_Compatible_word))

ifeq ($(Xavier_word), $(filter $(Xavier_word),$(dt_model)))
    MODEL := Xavier
else ifeq ($(Xavier_other_word_1), $(filter $(Xavier_other_word_1),$(dt_model)))
    MODEL := Xavier
else ifeq ($(Orin_word), $(filter $(Orin_word),$(dt_model)))
    ifeq ($(EAC_5000_word), $(filter $(EAC_5000_word),$(dt_model)))
        MODEL := EAC-5000
    else ifeq ($(Anvil_word), $(findstring $(Anvil_word),$(dt_model)))
        MODEL := $(Anvil_Model_word)

    else ifeq ($(Orin_Compatible_word), $(findstring $(Orin_Compatible_word),$(dt_compatible)))
        MODEL := Orin
    else
        MODEL := Not_Supported_Orin
    endif
else ifeq ($(ROScube_word), $(filter $(ROScube_word),$(dt_model)))
    ifeq ($(ROScube_Orin_word), $(filter $(ROScube_Orin_word),$(dt_model)))
        MODEL := ROScube-Orin
    else
        MODEL := ROScube-X
    endif
else
    MODEL := Not_Supported_Model_ROScube
endif

$(warning MODEL = $(MODEL))

ifeq ($(L4T_RELEASE_MAIN), R32)
    ifeq ($(L4T_RELEASE_MAJOR), 5)
        ifeq ($(MODEL), $(ROScube_word))
            KERNELRELEASE            := 4.9.201-rqx58g
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-roscube-r325x
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-roscube-r325x
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-roscube-r325x
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_roscube.py r32.5.1
        else ifeq ($(MODEL), $(Xavier_word))
            KERNELRELEASE            := 4.9.201-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-xavier-devkit-r325x
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-xavier-devkit-r325x
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-xavier-devkit-r325x
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_xavier-devkit.py  r32.5.1
        else
            KERNELRELEASE := Not_Supported
        endif
    else ifeq ($(L4T_RELEASE_MAJOR), 6)
        ifeq ($(MODEL),$(ROScube_word))
            KERNELRELEASE            := 4.9.253-rqx58g
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-roscube-r325x
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-roscube-r325x
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-roscube-r325x
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_roscube.py r32.5.1
        else ifeq ($(MODEL),$(Xavier_word))
            KERNELRELEASE            := 4.9.253-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-xavier-devkit-r325x
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-xavier-devkit-r325x
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-xavier-devkit-r325x
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_xavier-devkit.py  r32.5.1
        else
            KERNELRELEASE            := 4.9.253-unknown
            TARGET_ISX021_DTS        := Unknown
            TARGET_ISX021_IMX490_DTS := Unknown
            TARGET_IMX490_DTS        := Unknown
        endif
    endif
else ifeq ($(L4T_RELEASE_MAIN), R34)
    ifeq ($(L4T_RELEASE_MAJOR), 1)
        ifeq ($(MODEL),$(ROScube_word))
            KERNELRELEASE            := 5.10.65-rqx58g
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-roscube-r351
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-roscube-r351
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-roscube-r351
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_roscube.py r35.1
        else ifeq ($(MODEL),$(Xavier_word))
            KERNELRELEASE            := 5.10.65-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-xavier-devkit-r351
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-xavier-devkit-r351
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-xavier-devkit-r351
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_xavier-devkit.py  r35.1
        else ifeq ($(MODEL),$(Orin_word))
            KERNELRELEASE            := 5.10.65-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-orin-devkit-r351
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-orin-devkit-r351
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-orin-devkit-r351
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_orin-devkit.py  r35.1
        else
            KERNELRELEASE            := 5.10.65-unknown
            TARGET_ISX021_DTS        := Unknown
            TARGET_ISX021_IMX490_DTS := Unknown
            TARGET_IMX490_DTS        := Unknown
        endif
    endif
else ifeq ($(L4T_RELEASE_MAIN), R35)
        ifeq ($(MODEL),$(ROScube_word))
            KERNELRELEASE            := 5.10.120-rqx58g
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-roscube-r3541
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-roscube-r3541
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-roscube-r3541
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_roscube.py r35.4.1
        else ifeq ($(MODEL),$(Xavier_word))
            KERNELRELEASE            := 5.1.104-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-xavier-devkit-r351
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-xavier-devkit-r351
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-xavier-devkit-r351
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_xavier-devkit.py r35.1
        else ifeq ($(MODEL),$(Orin_word))
            ifeq ($(L4T_RELEASE_MAJOR), 1)
                KERNELRELEASE            := 5.1.104-tegra
                TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-orin-devkit-r351
                TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-orin-devkit-r351
                TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-orin-devkit-r351
                MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_orin-devkit.py  r35.1
            else ifeq ($(L4T_RELEASE_MAJOR), 2)
                KERNELRELEASE            := 5.1.104-tegra
                TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_orin-devkit.py  r35.2.1
            else ifeq ($(L4T_RELEASE_MAJOR), 3)
                KERNELRELEASE            := 5.10.104-tegra
                TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_orin-devkit.py  r35.2.1
            else ifeq ($(L4T_RELEASE_MAJOR), 4) # 35.4.1
                KERNELRELEASE                   := 5.10.104-tegra
                TARGET_ISX021_DTS               := tier4-isx021-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_IMX490_DTS               := tier4-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_IMX728_DTS               := tier4-imx728-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_ISX021_IMX490_DTS        := tier4-isx021-imx490-gmsl-device-tree-overlay-orin-devkit-r3521
                TARGET_ISX021_IMX490_IMX728_DTS := tier4-isx021-imx490-imx728-gmsl-device-tree-overlay-orin-devkit-r3521
                MAKE_OVERLAY_DTS_CMD            := make_overlay_dts_orin-devkit.py  r35.4.1
            else
                KERNELRELEASE            := Unknown
                TARGET_ISX021_DTS        := Unknown
                TARGET_ISX021_IMX490_DTS := Unknown
                TARGET_IMX490_DTS        := Unknown
            endif
        else ifeq ($(MODEL),$(ROScube_Orin_Model_word))
            KERNELRELEASE            := 5.10.104-rqx59x
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-roscube-orin-r351
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-roscube-orin-r351
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-roscube-orin-r351
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_roscube-orin.py r35.1
        else ifeq ($(MODEL),$(EAC_5000_word))
            KERNELRELEASE            := 5.10.104-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-eac-5000-r3531
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-eac-5000-r3531
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-eac-5000-r3531
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_eac-5000.py r35.3.1
        else ifeq ($(MODEL),$(Anvil_Model_word))
            KERNELRELEASE            := 5.10.104-tegra
            TARGET_ISX021_DTS        := tier4-isx021-gmsl-device-tree-overlay-anvil-r35
            TARGET_IMX490_DTS        := tier4-imx490-gmsl-device-tree-overlay-anvil-r35
            TARGET_IMX728_DTS        := tier4-imx728-gmsl-device-tree-overlay-anvil-r35
            TARGET_ISX021_IMX490_DTS := tier4-isx021-imx490-gmsl-device-tree-overlay-anvil-r35
            TARGET_ISX021_IMX490_IMX728_DTS := tier4-isx021-imx490-imx728-gmsl-device-tree-overlay-anvil-r35
            MAKE_OVERLAY_DTS_CMD     := make_overlay_dts_anvil.py r35 
            
        else
            KERNELRELEASE            := 5.1.104-unknown
            TARGET_ISX021_DTS        := Unknown
            TARGET_ISX021_IMX490_DTS := Unknown
            TARGET_IMX490_DTS        := Unknown
        endif
else
    KERNELRELEASE := Unknown
    TARGET_ISX021_DTS        := Unknown
    TARGET_ISX021_IMX490_DTS := Unknown
    TARGET_IMX490_DTS        := Unknown
endif

# ------------------------------------------------

#ifdef KERNEL_SRC_ROOT
#   KERNEL_SRC_DIR  := $(ROOTFS_DIR)/usr/src/linux-headers-5.10.104-tegra-ubuntu20.04_aarch64/kernel-5.10
#    KERNEL_SRC_DIR  := $(KERNEL_SRC_ROOT)
#else
    KERNEL_SRC_DIR  ?= /lib/modules/$(shell uname -r)/build
#endif

$(warning L4T_RELEASE    = $(L4T_RELEASE))
$(warning KERNELRELEASE  = $(KERNELRELEASE))
$(warning KERNEL_SRC_DIR = $(KERNEL_SRC_DIR))

$(warning TARGET_ISX021_DTS        = $(TARGET_ISX021_DTS))
$(warning TARGET_IMX490_DTS        = $(TARGET_IMX490_DTS))
$(warning TARGET_ISX021_IMX490_DTS = $(TARGET_ISX021_IMX490_DTS))

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

obj-m := tier4-max9295.o tier4-max9296.o tier4-isx021.o tier4-imx490.o tier4-gw5300.o tier4-fpga.o

# ------------------------------------------------

.PHONY: all
all: binary_module modules dtbo

modules:
	$(MAKE) -C $(KERNEL_SRC_DIR) ARCH=$(ARCH) M=$(BUILD_DIR) src=$(PWD) modules

binary_module:
	install -d $(DEST_FIRMWARE_DIR)
	cp -f $(SRC_FIRMWARE_BINARY) $(DEST_FIRMWARE_BINARY)

$(TARGET_ISX021_DTS).dts $(TARGET_IMX490_DTS).dts  $(TARGET_ISX021_IMX490_DTS).dts:

	python3 ./$(MAKE_OVERLAY_DTS_CMD) -8 C1
	python3 ./$(MAKE_OVERLAY_DTS_CMD) -8 C2
	python3 ./$(MAKE_OVERLAY_DTS_CMD) -2 C1 -2 C2 -2 C1 -2 C2

ifeq ($(L4T_RELEASE_MAIN), R34)
	sed -i -e '/^ *jetson-header-name/s/AGX  *CSI/AGX Xavier CSI/' $(TARGET_ISX021_DTS).dts
	sed -i -e '/^ *jetson-header-name/s/AGX  *CSI/AGX Xavier CSI/' $(TARGET_ISX021_IMX490_DTS).dts
	sed -i -e '/^ *jetson-header-name/s/AGX  *CSI/AGX Xavier CSI/' $(TARGET_IMX490_DTS).dts
else ifeq ($(L4T_RELEASE_MAIN), R35)
	sed -i -e '/^ *jetson-header-name/s/AGX Xavier CSI/AGX CSI/' $(TARGET_ISX021_DTS).dts
	sed -i -e '/^ *jetson-header-name/s/AGX Xavier CSI/AGX CSI/' $(TARGET_ISX021_IMX490_DTS).dts
	sed -i -e '/^ *jetson-header-name/s/AGX Xavier CSI/AGX CSI/' $(TARGET_IMX490_DTS).dts
endif

dtbo: $(TARGET_ISX021_DTS).dts $(TARGET_IMX490_DTS).dts  $(TARGET_ISX021_IMX490_DTS).dts


	dtc -O dtb -o $(TARGET_ISX021_DTS).dtbo -@ $(TARGET_ISX021_DTS).dts
	dtc -O dtb -o $(TARGET_IMX490_DTS).dtbo -@ $(TARGET_IMX490_DTS).dts
	dtc -O dtb -o $(TARGET_ISX021_IMX490_DTS).dtbo -@ $(TARGET_ISX021_IMX490_DTS).dts
	cp -rf $(TARGET_ISX021_DTS).dtbo /boot
	cp -rf $(TARGET_IMX490_DTS).dtbo /boot
	cp -rf $(TARGET_ISX021_IMX490_DTS).dtbo /boot

clean:
	$(MAKE) -C $(KERNEL_SRC_DIR) ARCH=$(ARCH) M=$(BUILD_DIR) src=$(PWD) clean
