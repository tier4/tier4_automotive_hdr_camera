#!/bin/bash
OUTPUT_MODULE_DIR=build/drivers
OUTPUT_IMAGE_DIR=build/kernel

mkdir -p $OUTPUT_MODULE_DIR
mkdir -p $OUTPUT_IMAGE_DIR

cp kernel/kernel-4.9/drivers/media/i2c/tier4-*.ko $OUTPUT_MODULE_DIR
cp kernel/kernel-4.9/arch/arm64/boot/Image $OUTPUT_IMAGE_DIR
cp kernel/kernel-4.9/arch/arm64/boot/dts/tegra194-p2888-0001-tier4-isx021.dtb $OUTPUT_IMAGE_DIR
