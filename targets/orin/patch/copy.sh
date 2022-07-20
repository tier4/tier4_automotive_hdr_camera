#!/bin/bash

ROOT_DIR=$1
TARGET_DIR=$2

COMMON_DTSI=hardware/nvidia/platform/t23x/common/kernel-dts/t234-common-modules/tegra234-camera-isx021-a00.dtsi   
OVERLAY=hardware/nvidia/platform/t23x/concord/kernel-dts/tegra234-p3737-camera-isx021-overlay.dts
CVB_DTSI=hardware/nvidia/platform/t23x/concord/kernel-dts/cvb/tegra234-p3737-0000-camera-isx021-a00.dtsi
CAMODULE_DTSI=hardware/nvidia/platform/t23x/concord/kernel-dts/cvb/tegra234-p3737-camera-modules.dtsi
cp -v ${ROOT_DIR}/${COMMON_DTSI} ${TARGET_DIR}/${COMMON_DTSI}
cp -v ${ROOT_DIR}/${CVB_DTSI} ${TARGET_DIR}/${CVB_DTSI}
cp -v ${ROOT_DIR}/${CAMODULE_DTSI} ${TARGET_DIR}/${CAMODULE_DTSI}
cp -v ${ROOT_DIR}/${OVERLAY} ${TARGET_DIR}/${OVERLAY}
