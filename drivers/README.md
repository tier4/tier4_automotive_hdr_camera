# tier4-camera-gmsl-dkms

This driver has been developped on ROScube RQX58G/Jetson Orin Devkit, it could not be cross compiled at this moment. 

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
$> sudo apt install ./tier4-camera-gmsl_x.x.x_arm64.deb
```

## Combine device-tree-overlay.dtbo with original dtb


``` 
$> sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="TIERIV ISX021 GMSL2 Camera Device Tree Overlay"
```

if you use r32.5.1 BSP, you should use the command below

``` 
$> sudo /opt/nvidia/jetson-io/config-by-hardware.py -n "TIERIV ISX021 GMSL2 Camera Device Tree Overlay"
```


## Confirm /boot/kernel_tegra194-*-user-custom.dtb has been generated

```
$> ls  /boot/kernel_tegra194-*-user-custom.dtb
```

if you use r32.5.1 BSP, you should use the command below

```
$> ls /boot/kernel_tegra194-rqx-58g-*.dtb
```


## Then, shutdown the system

$> sudo shutdown -h now

## Power on again

