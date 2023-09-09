# tier4-camera-gmsl-dkms

This driver has been developed on ROScube RQX58G/Jetson Orin Devkit, it could not be cross-compiled at this moment. 

## Preparation

``` sh
sudo apt update
sudo apt install make build-essential debhelper debmake devscripts dkms
```

## Build debian-dkms package

``` sh
dpkg-buildpackage -b -rfakeroot -us -uc
```

## Install the debian-dkms package

``` sh
cd ..
sudo apt update
sudo apt install ./tier4-isx021-gmsl_1.4.1_arm64.deb
```

## Confirm the dtbo file exists

```
ls /boot/*.dtbo
```


## Combine dtbo file with vanilla dtb


``` sh
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="TIERIV ISX021 GMSL2 Camera Device Tree Overlay"
```

if you use r32.5.1 BSP, you should use the command below

``` sh
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n "TIERIV ISX021 GMSL2 Camera Device Tree Overlay"
```


## Confirm custom dtb file has been generated

``` sh
ls -al  /boot/*user-custom.dtb
```


## Then, shutdown the system

$> sudo shutdown -h now

## Power on again