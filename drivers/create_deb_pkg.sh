#!/bin/bash

if [ -e ./src/tier4-isx021.bin ]
then
  grep -q "TIER4_ISX021_REG_0_ADDR" ./src/tier4-isx021.c
  if [ ! $? = 0 ]
  then
    echo "The register address is not hidden."
    echo "exec python ./tool/gen_firmware.py ./src/tier4-isx021.c"
    python3 ./tools/gen_firmware.py ./src/tier4-isx021.c
  fi
  dpkg-buildpackage -b -rfakeroot -us -uc
fi

#dpkg-buildpackage -b -rfakeroot -us -uc
