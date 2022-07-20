#!/bin/bash

if [ -e ./src/tier4-isx021.bin ]
then
  grep -q "TIER4_ISX021_REG_0_ADDR" ./src/tier4-isx021.c
  if [ $? = 0 ]
  then
    dpkg-buildpackage -b -rfakeroot -us -uc
  else
    echo "The register address is not hidden.please confirm."
  fi
fi

#dpkg-buildpackage -b -rfakeroot -us -uc
