#!/bin/bash -e
help()
{
    echo "DKMS driver install script for RQX-58G" 
    echo
    echo "Syntax: ./install.sh"
}

while getopts ":h:" option; do
    case $option in
        h) # Display help
            help
            exit;;
        \?) # Invalid option
            echo "Error: Invalid option"
            exit;;
    esac
done

# Specify driver version here to avoid to install any unintended driver version
DRIVER_VER=1.0.2

SCRIPT_DIR=$(cd $(dirname $0); pwd)
DATE=`date +'%y%m%d'`
TIME=`date +'%H%M%S'`
INSTALL_LOG=$SCRIPT_DIR/installation-${DATE}-${TIME}.log
exec 1> >( tee "${INSTALL_LOG}" ) 2>&1

if [ ! -e ${SCRIPT_DIR}/tier4-isx021-gmsl_${DRIVER_VER}_arm64.deb ]; then
    echo -e "\n ERROR: ${SCRIPT_DIR}/tier4-isx021-gmsl_${DRIVER_VER}_arm64.deb does not exist."
    exit 1
fi

# Create empty file to log installation date and time
INSTALL_TIME_FILE=/tmp/tier4-isx021-gmsl-install_${DATE}${TIME}
touch $INSTALL_TIME_FILE

echo "-----Show installation info-----"
echo -n "**Driver deb package info: "
echo `md5sum ${SCRIPT_DIR}/tier4-isx021-gmsl_${DRIVER_VER}_arm64.deb` 
echo "**System information (uname -a):"
uname -a
echo "**tegra release ver (cat /etc/nv_tegra_release)"
cat /etc/nv_tegra_release 
echo "**FPGA information (i2cdetect -y -r 2)"
i2cdetect -y -r 2

echo "-----Install required packages-----"
sudo apt update
sudo apt install -y make debhelper dkms

echo "-----Install driver package-----"
cd $SCRIPT_DIR
sudo apt install ./tier4-isx021-gmsl_${DRIVER_VER}_arm64.deb

echo "-----Confirm /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo exists-----"
ls -l /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo

echo "-----Combine /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo with /boot/dtb/kernel_tegra194-rqx-58g.dtb-----"
if [ `grep 'R32 (release), REVISION: 5' /etc/nv_tegra_release -c` -eq 1 ]; then
    sudo /opt/nvidia/jetson-io/config-by-hardware.py -n "Tier4 ISX021 GMSL2 Camera Device Tree Overlay"
    DTB_FILE=/boot/kernel_tegra194-rqx-58g-tier4-isx021-gmsl2-camera-device-tree-overlay.dtb
elif [ `grep 'R32 (release), REVISION: 6' /etc/nv_tegra_release -c` -eq 1 ]; then
    sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Tier4 ISX021 GMSL2 Camera Device Tree Overlay"
    DTB_FILE=/boot/kernel_tegra194-rqx-58g-user-custom.dtb
else
    echo "ERROR: L4T version does not match to any supported versions."
    exit 1
fi

echo "-----Confirm ${DTB_FILE} has been generated-----"
if [ ! -e $DTB_FILE ]; then
    echo "ERROR: ${DTB_FILE} does not exist. Please check the driver version and L4T version."
    exit 1
fi
# If .dtb file is older than the start time of this install script.
if [ $DTB_FILE -ot $INSTALL_TIME_FILE ]; then
    echo "ERROR: ${DTB_FILE} was not updated. Please check the driver version and L4T version."
    exit 1
fi
ls -l $DTB_FILE

echo "-----Check additional descriptions in /boot/extlinux/extlinux.conf-----"
cat /boot/extlinux/extlinux.conf
if [ `grep "FDT ${DTB_FILE}" /boot/extlinux/extlinux.conf -c` -eq 0 ]; then
    echo "ERROR: Could not found additional descriptions in /boot/extlinux/extlinux.conf"
    exit 1
fi

echo "---------------------------------------------"
echo "Installation and configuration done. Please reboot by \"sudo shutdown -h now\""
