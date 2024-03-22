#!/bin/bash
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 board L4T-revision overlay-args..."
    echo "  board = orin-devkit, roscube, roscube-orin xavier-devkit"
    echo "  L4T-revision = R32.5.1, R32.5.2, R32.6.1, R35.1 ..."
    exit 1
fi

BOARD=$1
L4T_REVISION=$2
SCRIPT_PATH="$(realpath $0)"
cd "$(dirname $SCRIPT_PATH)"

set -eux

mkdir -p overlay
cd overlay
rm -f *.dts *.dtbo
"../../../../drivers/src/make_overlay_dts_$BOARD.py" "$L4T_REVISION" ${@:3}
dtc -O dtb -o target.dtbo -@ $(ls *.dts)

sudo modprobe -r tier4-isx021 || true
sudo modprobe -r tier4-imx490 || true
sudo modprobe -r tier4-imx728 || true
sudo rmdir /sys/kernel/config/device-tree/overlays/camera/ || true

sudo mkdir -p /sys/kernel/config/device-tree/overlays/camera/
sudo cp target.dtbo /sys/kernel/config/device-tree/overlays/camera/dtbo
echo 1 | sudo tee /sys/kernel/config/device-tree/overlays/camera/status

sudo modprobe tier4-imx728
