#!/bin/sh
set -eu

[ $# -eq 1 ] || exit 1
[ -e $1 ] || exit 1

CAMERA_PATH=$(v4l2-ctl --info -d $1 | grep "Card type" | grep -o "[0-9]\+-[0-9a-f]\+")
CAMERA_OF_NODE=$(cat $(find /sys/devices/ -name ${CAMERA_PATH})/of_node/name)
SENSOR=$(echo $CAMERA_OF_NODE | cut -d _ -f1)

INDEX=$(printf '%d' "'$(echo ${CAMERA_OF_NODE} | cut -d _ -f2)")
INDEX_START=$(printf '%d' "'a")
NUM_INDEX=$(expr ${INDEX} - ${INDEX_START} + 1)

GMSL_DIR="/dev/gmsl"
mkdir -p "${GMSL_DIR}"
ln -s $1 "${GMSL_DIR}/tier4-${SENSOR}-cam${NUM_INDEX}"
