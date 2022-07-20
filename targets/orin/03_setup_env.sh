# set your bsp
#export BSP_BASE_DIR=

#export DRVPKG_DIR=${BSP_BASE_DIR}/Linux_for_Tegra

# if you need to make Roof File System
#export ROOTFS_DIR=${DRVPKG_DIR}/rootfs

#export KERNEL_SRC=a/source/public/kernel/kernel-4.9

export CROSS_COMPILE="/opt/l4t-gcc/bin/aarch64-buildroot-linux-gnu-"
export ARCH=arm64

export LOCAL_VERSION=-tegra
export LOCALVERSION=-tegra

# kernelやmoduleやDTBがのビルドされたファイルの格納場所のtop directory

#export BUILD_DIR=${BSP_BASE_DIR}/build
#
#export KERNEL_OUT=${BUILD_DIR}/kernel_out
#export MODULE_OUT=${BUILD_DIR}/module_out
#export K_OUT=${KERNEL_OUT}
#export M_OUT=${MODULE_OUT}

# kernel image : ${KERNEL_OUT}/arch/arm64/boot/Image
# DTB          : ${KERNEL_OUT}/arch/arm64/boot/dts/tegra194-p2888-0001-leopard-isx021.dtb
#

### for boot ###
# /boot/extlinux/extlinux.conf
# FDT   /boot/tegra194-p2888-0001-leopard-isx021.dtb
# LINUX /boot/Image
