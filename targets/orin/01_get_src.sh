#!/bin/bash

# r32.5.1
if [ ! -e public_sources.tbz2 ]
then
wget https://developer.nvidia.com/embedded/l4t/r34_release_v1.1/sources/public_sources.tbz2
fi

tar xvf public_sources.tbz2
tar xvf Linux_for_Tegra/source/public/kernel_src.tbz2 
