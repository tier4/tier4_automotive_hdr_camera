# tier4-isx021-gmsl-dkms

This driver has been developped on  ROScube RQX58G, it could  not be cross compiled at this moment. 

## Preparation

In this directory  

```
$> sudo apt update
$> sudo apt install make build-essential debhelper debmake devscripts dkms
```

## Build debian-dkms package

```
$> dpkg-buildpackage -b -rfakeroot -us -uc
```

## Install the debian--dkms package

```
$> cd ..
$> sudo apt update
$> sudo apt install ./tier4-isx021-gmsl_1.0.3_arm64.deb
```

## Confirm  /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo exists

```
$> ls /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo
```


## Combine /boot/tier4-isx021-gmsl-device-tree-overlay.dtbo with  /boot/dtb/kernel_tegra194-rqx-58g.dtb


``` 
$> sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Tier4 ISX021 GMSL2 Camera Device Tree Overlay"
```

if you use r32.5.1 BSP, you should use the command below

``` 
$> sudo /opt/nvidia/jetson-io/config-by-hardware.py -n "Tier4 ISX021 GMSL2 Camera Device Tree Overlay"
```


## Confirm /boot/kernel_tegra194-rqx-58g-user-custom.dtb has been generated

```
$> ls  /boot/kernel_tegra194-rqx-58g-user-custom.dtb
```

if you use r32.5.1 BSP, you should use the command below

```
$> ls /boot/kernel_tegra194-rqx-58g-tier4-isx021-gmsl2-camera-device-tree-overlay.dtb
```


## Then, shutdown the system

$> sudo shutdown -h now

## Power on again

