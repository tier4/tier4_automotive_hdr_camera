# TIERIV Automotive HDR Camera C1/C2 Device Driver
This repository contains the device driver for the [TIERIV Automotive HDR Camera C1/C2](https://sensor.tier4.jp/automotive-hdr-camera). 

If you want to use the official driver, please refer to the [Installation and Usage section](#installation-and-usage) and proceed with the installation.

If you want to modify the driver, please refer to the [How to create a package from source section](#create-debian-dkms-package).


## Installation and Usage

1. Download the latest release of the driver from the [releases page](https://github.com/tier4/tier4_automotive_hdr_camera/releases).
2. Follow the installation instructions provided in the quickstart guide.

## How to create a package from source 

### Preparation

```
$> sudo apt update
$> sudo apt install make build-essential debhelper debmake devscripts dkms 
```

### Create debian-dkms package

```
$> dpkg-buildpackage -b -rfakeroot -us -uc
```

You can confirm the package file with the following command.
```
$> ls ../*deb
```

## Contributing

We welcome contributions to the Automotive HDR Camera Device Driver. 
If you have a bug fix or new feature that you would like to contribute, please submit a issue or pull request.
