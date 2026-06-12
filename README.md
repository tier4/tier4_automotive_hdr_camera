# TIER IV Automotive HDR Camera Device Driver

This repository contains the device driver for the [TIER IV Automotive HDR Camera C1/C1MP, C2/C2MP, and C3](https://edge.auto/automotive-camera). 

If you want to use the official driver, please refer to the [Installation and Usage](#installation-and-usage) section and proceed with the installation.

If you want to modify the driver, please refer to the [Create a .deb package from source and install](#create-a-deb-package-from-source-and-install) section.

For the simple image parameter tuning tool (`t4cam-ctrl`), please refer to the documentation here: [t4cam-ctrl User Manual](https://tier4.github.io/edge-auto-docs/user_manual/camera-control-tool-user-manual.html).

## Installation and Usage

You can install the driver using one of the following two methods.

### Install from built .deb package

1. Download the latest release of the driver from the [releases page](https://github.com/tier4/tier4_automotive_hdr_camera/releases).
2. Follow the installation instructions provided in the [quickstart guide](https://tier4.github.io/edge-auto-docs/getting_started/index.html).

### Create a .deb package from source and install

You can build and install the package from the source code by running the following script:

```bash
# install dependencies
sudo apt update
sudo apt install make build-essential debhelper debmake devscripts dkms

# create .deb package
cd pkg
./create_deb_pkg.sh

# install driver from .deb package
sudo apt-get install -y ../*.deb
```

## Camera Assignment

The camera assigned to each GMSL2 port is fixed based on the device tree configuration. Therefore, you must modify the device tree settings to match the connected cameras.

### Default Device Tree Assignment

The device type (i.e., C1 or C2) assignment is fixed at every GMSL2 port. After installing the driver, the following device tree overlay files are automatically generated.

The following table shows the assignments:

| overlay-command                                                        | dtbo-path                                                       | GMSL2 port No. | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    |
| ---------------------------------------------------------------------- | --------------------------------------------------------------- | -------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1x8`                        | `/boot/tier4-c1-gmsl-device-tree-overlay-anvil-r36.dtbo`        |                | C1   | C1   | C1   | C1   | C1   | C1   | C1   | C1   |  
| `TIERIV GMSL2 Camera Device Tree Overlay: C2x8`                        | `/boot/tier4-c2-gmsl-device-tree-overlay-anvil-r36.dtbo`        |                | C2   | C2   | C2   | C2   | C2   | C2   | C2   | C2   |
| `TIERIV GMSL2 Camera Device Tree Overlay: C3x8`                        | `/boot/tier4-c3-gmsl-device-tree-overlay-anvil-r36.dtbo`        |                | C3   | C3   | C3   | C3   | C3   | C3   | C3   | C3   |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1MPx8`                      | `/boot/tier4-c1mp-gmsl-device-tree-overlay-anvil-r36.dtbo`      |                | C1MP | C1MP | C1MP | C1MP | C1MP | C1MP | C1MP | C1MP |
| `TIERIV GMSL2 Camera Device Tree Overlay: C2MPx8`                      | `/boot/tier4-c2mp-gmsl-device-tree-overlay-anvil-r36.dtbo`      |                | C2MP | C2MP | C2MP | C2MP | C2MP | C2MP | C2MP | C2MP |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1x4 C1MPx4`                 | `/boot/tier4-c1-c1mp-gmsl-device-tree-overlay-anvil-r36.dtbo`   |                | C1   | C1   | C1   | C1   | C1MP | C1MP | C1MP | C1MP |
| `TIERIV GMSL2 Camera Device Tree Overlay: C2x4 C2MPx4`                 | `/boot/tier4-c2-c2mp-gmsl-device-tree-overlay-anvil-r36.dtbo`   |                | C2   | C2   | C2   | C2   | C2MP | C2MP | C2MP | C2MP |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1x2 C2x2 C1x2 C2x2`         | `/boot/tier4-c1-c2-gmsl-device-tree-overlay-anvil-r36.dtbo`     |                | C1   | C1   | C2   | C2   | C1   | C1   | C2   | C2   |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1MPx2 C2MPx2 C1MPx2 C2MPx2` | `/boot/tier4-c1mp-c2mp-gmsl-device-tree-overlay-anvil-r36.dtbo` |                | C1MP | C1MP | C2MP | C2MP | C1MP | C1MP | C2MP | C2MP |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1x2 C2x2 C3x4`              | `/boot/tier4-c1-c3-gmsl-device-tree-overlay-anvil-r36.dtbo`     |                | C1   | C1   | C2   | C2   | C3   | C3   | C3   | C3   |
| `TIERIV GMSL2 Camera Device Tree Overlay: C1x2 C3x2 C1x2 C3x2`         | `/boot/tier4-c2-c3-gmsl-device-tree-overlay-anvil-r36.dtbo`     |                | C1   | C1   | C3   | C3   | C1   | C1   | C3   | C3   |
| `TIERIV GMSL2 Camera Device Tree Overlay: C2x2 C3x2 C2x2 C3x2`         | `/boot/tier4-c1-c2-c3-gmsl-device-tree-overlay-anvil-r36.dtbo`  |                | C2   | C2   | C3   | C3   | C2   | C2   | C3   | C3   |


After the driver installation process, execute following command and apply the camera assignments.

```bash
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n <header-num>=<overlay-command>
```

* For example, if you execute `$ sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="TIERIV GMSL2 Camera Device Tree Overlay: C1x8"`, all ports are assigned for C1 cameras.
* For usage instructions, please also refer to the help option of `config-by-hardware.py`.

> [!NOTE]
> When using **Anvil**, the `config-by-hardware.py` script cannot be used. Instead, please edit the `/boot/extlinux/extlinux.conf` file directly as follows:
> 
> ``` diff
> - DEFAULT primary
> ---
> + DEFAULT JetsonIO
> 29a30,37
> + 
> + LABEL JetsonIO
> +       MENU LABEL primary kernel
> +       LINUX /boot/Image
> +       INITRD /boot/initrd
> +       APPEND ${cbootargs} root=/dev/mmcblk0p1 rw rootwait rootfstype=ext4 mminit_loglevel=4 console=ttyTCU0,115200 console=ttyAMA0,115200 firmware_class.path=/etc/firmware fbcon=map:0 video=efifb:off console=tty0
> +       FDT /boot/tegra234-orin-agx-cti-AGX201.dtb
> +       OVERLAYS /boot/<dtbo-path>
> ```

### Creating a Custom Device Tree

If you want to apply a configuration other than the default assignments, follow the steps below to create and apply a custom device tree:

1. **Generate a `dts` file to describe your desired assignment**

    ```bash
    $ cd tools/dts_generator/
    # Specify device type assignment from port #1 to #8 one by one
    $ python3 make_overlay_dts_$(DEVICE_NAME).py R36.4.3 -2 c1 -4 C2 -2 C2
    ```
    - By the above example, a file named `tier4-isx021-imx490-device-tree-overlay-$(DEVICE_NAME).dts` will be created.

    * Please modify the `make_overlay_dts_anvil.py` file appropriately for your specific machine.
    * The options `-2`, `-4`, `-6`, and `-8` specify how many cameras of the given type are connected sequentially.
    * By the above example, 4 units of C1 and 4 units of C2 will be assigned, and a file named `tier4-c1-c2-gmsl-device-tree-overlay-anvil-r36.dts` will be created.


2. **Generate `dtbo` from `dts`**

    ```bash
    dtc -O dtb -o ${OVERLAY_DTBO_FILE} -@ ${OVERLAY_DTS_FILE}
    ```

    * `${OVERLAY_DTBO_FILE}` indicates the desired output filename (replacing the file extension from `.dts` to `.dtbo`).
    * e.g., `tier4-c1-c2-gmsl-device-tree-overlay-anvil-r36.dtbo`
　  * `${OVERLAY_DTS_FILE}` is the `.dts` file generated in the previous step.

3. **Copy the result to the appropriate directory and run configure**

    ```bash
    sudo cp ${OVERLAY_DTBO_FILE} /boot/
    ```


4. **Shutdown and reboot the system**

    ```bash
    sudo shutdown -h now
    ```

    Then, reboot (turn on) the system manually.

## Contribution

We welcome contributions to the Automotive HDR Camera Device Driver.
If you have a bug fix or new feature that you would like to contribute, please submit an issue or a pull request.
