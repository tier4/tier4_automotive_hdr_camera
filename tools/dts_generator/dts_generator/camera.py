#!/usr/bin/env python3

import abc
from enum import Enum
from typing import Dict, List, Optional, SupportsInt, Tuple

from . import StringLike, gmsl, validate_csi_lanes
from .devicetree import DeviceTreeNode

PORT_ENDPOINT_LABEL: str = 'cam_out'


class Type(Enum):
    C1 = 1
    C2 = 2
    C3 = 3

    @property
    def image_sensor_name(self) -> str:
        return _image_sensor_name[self]


_image_sensor_name: Dict[Type, str] = {
    Type.C1: 'isx021',
    Type.C2: 'imx490',
    Type.C3: 'imx728',
}


def _camera_gmsl_serializer(index: int, suffix: StringLike, vc_id: StringLike,
                            dser_label: str, primary: bool
                            ) -> Tuple[Optional[DeviceTreeNode], DeviceTreeNode]:
    if primary:
        return (gmsl.Serializer.primary(index),
                gmsl.Serializer.secondary_a(index, suffix, vc_id, dser_label))
    else:
        return (None, gmsl.Serializer.secondary_b(index, suffix, vc_id, dser_label))


def _camera_isp(index: int, suffix: StringLike, vc_id: StringLike,
                dser_label: str, primary: bool
                ) -> Tuple[Optional[DeviceTreeNode], DeviceTreeNode]:
    isp_prim = isp = None

    if primary:
        isp_prim = (DeviceTreeNode('gw5300_prim@6d', label=f'isp{index}_prim')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_gw5300"',
                'reg = <0x6d>',
                'is-prim-isp',
            ]))
        isp = (DeviceTreeNode(f'gw5300_{suffix}@6e', label=f'isp{index}_a')
            .properties([
                'compatible = "nvidia,tier4_gw5300"',
                'status = "okay"',
                'reg = <0x6e>',
            ]))
    else:
        isp_prim = None
        isp = (DeviceTreeNode(f'gw5300_{suffix}@6f', label=f'isp{index}_b')
            .properties([
                'compatible = "nvidia,tier4_gw5300"',
                'status = "okay"',
                'reg = <0x6f>',
            ]))

    return (isp_prim, isp)


class Camera(abc.ABC):
    @staticmethod
    @abc.abstractmethod
    def camera_type() -> Type:
        pass

    @property
    @abc.abstractmethod
    def endpoint(self) -> DeviceTreeNode:
        pass

    @property
    @abc.abstractmethod
    def image_sensor(self) -> DeviceTreeNode:
        pass

    @property
    @abc.abstractmethod
    def sensor_address(self) -> str:
        pass

    @abc.abstractmethod
    def to_list(self) -> List[Optional[DeviceTreeNode]]:
        pass


class C1(Camera):
    @staticmethod
    def camera_type() -> Type:
        return Type.C1

    @property
    def endpoint(self) -> DeviceTreeNode:
        return self._endpoint

    @property
    def image_sensor(self) -> DeviceTreeNode:
        return self._image_sensor

    @property
    def sensor_address(self) -> str:
        return self._sensor_address

    def __init__(self, index: int, suffix: StringLike, address: str, vc_id: StringLike,
                 csi_lanes: SupportsInt, port_index: StringLike, serdes_csi_link: str,
                 dser_label: str, serdes_pix_clk_hz: SupportsInt, primary: bool) -> None:
        serdes_pix_clk_hz = int(serdes_pix_clk_hz)
        csi_lanes = int(csi_lanes)
        validate_csi_lanes(csi_lanes)

        self.index = index
        self._sensor_address = address

        self.ser_prim, self.ser = _camera_gmsl_serializer(index, suffix, vc_id, dser_label, primary)

        self._endpoint = (DeviceTreeNode('endpoint', label=f'{PORT_ENDPOINT_LABEL}{index}')
            .properties([
                f'vc-id = <{vc_id}>',
                f'port-index = <{port_index}>',
                f'bus-width = <{csi_lanes}>',
                'remote-endpoint = <&csi_in>',  # Should be updated later by `MediaGraph`
            ]))

        self._image_sensor = (DeviceTreeNode(f'isx021_{suffix}@{address}')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_isx021"',
                'def-addr = <0x1a>',
                'mclk = "extperiph1"',
                'clocks = <&bpmp 36U>, <&bpmp 36U>',
                'clock-names = "extperiph1", "pllp_grtba"',
                f'nvidia,gmsl-ser-device = <&{self.ser.node_label}>',
                f'nvidia,gmsl-dser-device = <&{dser_label}>',
                # 'nvidia,fpga-device  = <&t4_fpga>',
                f'reg = <0x{address}>',
                'physical_w = "15.0"',
                'physical_h = "12.5"',
                'sensor_model = "isx021"',
                'fsync-mode = "false"',
                'distortion-correction = "false"',
                'auto-exposure = "true"',
                'post_crop_frame_drop = "0"',
                'use_decibel_gain = "true"',
                'use_sensor_mode_id = "false"',
                f'reg_mux = <{int(index / 2)}>'
            ])
            .nodes([
                DeviceTreeNode('mode0')
                    .header('\n/* mode ISX021_MODE_1920X1280_CROP_30FPS */')
                    .properties([
                        f'num_lanes = "{csi_lanes}"',
                        f'vc_id = "{csi_lanes}"',
                        f'serdes_pix_clk_hz = "{serdes_pix_clk_hz}"',
                        'mclk_khz = "24000"',
                        'tegra_sinterface = "serial_a"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',
                        'active_w = "1920"',
                        'active_h = "1280"',
                        'readout_orientation = "0"',
                        'line_length = "2250"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "375000000"',
                        'gain_factor = "10"',
                        'min_gain_val = "0"',       # dB
                        'max_gain_val = "300"',     # dB
                        'step_gain_val = "3"',      # 0.3
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "30000000"',
                        'max_framerate = "30000000"',
                        'step_framerate = "1"',
                        'default_framerate = "30000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',  # us 1 line
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',   # us
                        'embedded_metadata_height = "0"',
                    ]),
                DeviceTreeNode('mode1')
                    .header('\n/* mode ISX021_MODE_1920X1280_CROP_30FPS with Front Embedded data */')
                    .properties([
                        f'num_lanes = "{csi_lanes}"',
                        f'vc_id = "{csi_lanes}"',
                        f'serdes_pix_clk_hz = "{serdes_pix_clk_hz}"',
                        'mclk_khz = "24000"',
                        'tegra_sinterface = "serial_a"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',
                        'active_w = "1920"',
                        'active_h = "1281"',
                        'readout_orientation = "0"',
                        'line_length = "2250"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "375000000"',
                        'gain_factor = "10"',
                        'min_gain_val = "0"',
                        'max_gain_val = "300"',
                        'step_gain_val = "3"',
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "30000000"',
                        'max_framerate = "30000000"',
                        'step_framerate = "1"',
                        'default_framerate = "30000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',
                        'embedded_metadata_height = "0"',
                    ]),
                DeviceTreeNode('mode2')
                    .header('\n/* mode ISX021_MODE_1920X1280_CROP_30FPS with Rear Embedded data */')
                    .properties([
                        f'num_lanes = "{csi_lanes}"',
                        f'vc_id = "{csi_lanes}"',
                        f'serdes_pix_clk_hz = "{serdes_pix_clk_hz}"',
                        'mclk_khz = "24000"',
                        'tegra_sinterface = "serial_a"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',
                        'active_w = "1920"',
                        'active_h = "1294"',
                        'readout_orientation = "0"',
                        'line_length = "2250"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "375000000"',
                        'gain_factor = "10"',
                        'min_gain_val = "0"',
                        'max_gain_val = "300"',
                        'step_gain_val = "3"',
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "30000000"',
                        'max_framerate = "30000000"',
                        'step_framerate = "1"',
                        'default_framerate = "30000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',
                        'embedded_metadata_height = "0"',
                    ]),
                DeviceTreeNode('mode3')
                    .header('\n/* mode ISX021_MODE_1920X1280_CROP_30FPS with Front and Rear Embedded data */')
                    .properties([
                        f'num_lanes = "{csi_lanes}"',
                        f'vc_id = "{csi_lanes}"',
                        f'serdes_pix_clk_hz = "{serdes_pix_clk_hz}"',
                        'mclk_khz = "24000"',
                        'tegra_sinterface = "serial_a"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',
                        'active_w = "1920"',
                        'active_h = "1295"',
                        'readout_orientation = "0"',
                        'line_length = "2250"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "375000000"',
                        'gain_factor = "10"',
                        'min_gain_val = "0"',
                        'max_gain_val = "300"',
                        'step_gain_val = "3"',
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "30000000"',
                        'max_framerate = "30000000"',
                        'step_framerate = "1"',
                        'default_framerate = "30000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',
                        'embedded_metadata_height = "0"',
                    ]),
                DeviceTreeNode('ports')
                    .properties([
                        '#address-cells = <1>',
                        '#size-cells = <0>',
                    ])
                    .nodes([
                        DeviceTreeNode('port@0')
                            .properties([
                                'reg = <0>',
                            ])
                            .nodes([self.endpoint])
                    ]),
                DeviceTreeNode('gmsl-link')
                    .properties([
                        'src-csi-port = "b"',
                        'dst-csi-port = "a"',
                        f'serdes-csi-link = "{serdes_csi_link}"',
                        f'csi-mode = "1x{csi_lanes}"',
                        f'st-vc = <{vc_id}>',
                        f'vc-id = <{vc_id}>',
                        f'num-lanes = <{csi_lanes}>',
                        'streams = "ued-u1","yuv8"',
                    ]),
            ]))

    def to_list(self) -> List[Optional[DeviceTreeNode]]:
        return [self.ser_prim, self.ser, self.image_sensor]


class C2(Camera):
    @staticmethod
    def camera_type() -> Type:
        return Type.C2

    @property
    def endpoint(self) -> DeviceTreeNode:
        return self._endpoint

    @property
    def image_sensor(self) -> DeviceTreeNode:
        return self._image_sensor

    @property
    def sensor_address(self) -> str:
        return self._sensor_address

    def __init__(self, index: int, suffix: StringLike, address: str, vc_id: StringLike,
                 csi_lanes: SupportsInt, port_index: StringLike, serdes_csi_link: str,
                 dser_label: str, serdes_pix_clk_hz: SupportsInt, primary: bool) -> None:
        csi_lanes = int(csi_lanes)
        validate_csi_lanes(csi_lanes)

        self.index = index
        self._sensor_address = address

        self.ser_prim, self.ser = _camera_gmsl_serializer(index, suffix, vc_id, dser_label, primary)
        self.isp_prim, self.isp = _camera_isp(index, suffix, vc_id, dser_label, primary)

        self._endpoint = (DeviceTreeNode('endpoint', label=f'{PORT_ENDPOINT_LABEL}{index}')
            .properties([
                f'vc-id = <{vc_id}>',
                f'port-index = <{port_index}>',
                f'bus-width = <{csi_lanes}>',
                'remote-endpoint = <&csi_in>',  # Should be updated later by `MediaGraph`
            ]))

        self._image_sensor = (DeviceTreeNode(f'imx490_{suffix}@{address}')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_imx490"',
                f'reg = <0x{address}>',
                'def-addr = <0x1a>',
                'mclk = "extperiph1"',
                'clocks = <&bpmp 36U>, <&bpmp 36U>',
                'clock-names = "extperiph1","pllp_grtba"',
                f'nvidia,isp-device = <&{self.isp.node_label}>',
                f'nvidia,gmsl-ser-device = <&{self.ser.node_label}>',
                f'nvidia,gmsl-dser-device = <&{dser_label}>',
                'physical_w = "15.0"',
                'physical_h = "12.5"',
                'sensor_model = "imx490"',
                'fsync-mode = "false"',
                'distortion-correction = "false"',
                'auto-exposure = "true"',
                'post_crop_frame_drop = "0"',
                'use_decibel_gain = "true"',
                'use_sensor_mode_id = "true"',
                f'reg_mux = <{int(index / 2)}>'
            ])
            .nodes([
                DeviceTreeNode('mode0')
                    .header('\n/* mode IMX490_MODE_2880X1860_CROP_30FPS */')
                    .properties([
                        f'num_lanes = "{csi_lanes}"',
                        f'vc_id = "{csi_lanes}"',
                        'tegra_sinterface = "serial_g"',
                        'vc_id = "0"',
                        'tegra_sinterface = "serial_g"',
                        'mclk_khz = "24000"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'phy_mode = "DPHY"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',
                        'active_w = "2880"',
                        'active_h = "1860"',
                        'readout_orientation = "0"',
                        'line_length = "2250"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "375000000"',
                        f'serdes_pix_clk_hz = "{int(serdes_pix_clk_hz)}"',
                        'gain_factor = "10"',
                        'min_gain_val = "0"',
                        'max_gain_val = "300"',
                        'step_gain_val = "3"',
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "30000000"',
                        'max_framerate = "30000000"',
                        'step_framerate = "1"',
                        'default_framerate = "30000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',
                        'embedded_metadata_height = "0"',
                        # 'deskew_initial_enable = "true"',
                    ]),
                DeviceTreeNode('ports')
                    .properties([
                        '#address-cells = <1>',
                        '#size-cells = <0>',
                    ])
                    .nodes([
                        DeviceTreeNode('port@0')
                            .properties([
                                'reg = <0>',
                            ])
                            .nodes([self.endpoint])
                    ]),
                DeviceTreeNode('gmsl-link')
                    .properties([
                        'src-csi-port = "b"',
                        'dst-csi-port = "a"',
                        f'serdes-csi-link = "{serdes_csi_link}"',
                        f'csi-mode = "1x{csi_lanes}"',
                        f'st-vc = <{vc_id}>',
                        f'vc-id = <{vc_id}>',
                        f'num-lanes = <{csi_lanes}>',
                        'streams = "ued-u1","yuv8"',
                    ])
            ]))

    def to_list(self) -> List[Optional[DeviceTreeNode]]:
        return [self.ser_prim, self.ser, self.isp_prim, self.isp, self.image_sensor]


class C3(Camera):
    @staticmethod
    def camera_type() -> Type:
        return Type.C3

    @property
    def endpoint(self) -> DeviceTreeNode:
        return self._endpoint

    @property
    def image_sensor(self) -> DeviceTreeNode:
        return self._image_sensor

    @property
    def sensor_address(self) -> str:
        return self._sensor_address

    def __init__(self, index: int, suffix: StringLike, address: str, vc_id: StringLike,
                 csi_lanes: SupportsInt, port_index: StringLike, serdes_csi_link: str,
                 dser_label: str, serdes_pix_clk_hz: SupportsInt, primary: bool) -> None:
        csi_lanes = int(csi_lanes)
        validate_csi_lanes(csi_lanes)

        self.index = index
        self._sensor_address = address

        self.ser_prim, self.ser = _camera_gmsl_serializer(index, suffix, vc_id, dser_label, primary)
        self.isp_prim, self.isp = _camera_isp(index, suffix, vc_id, dser_label, primary)

        self._endpoint = (DeviceTreeNode('endpoint', label=f'{PORT_ENDPOINT_LABEL}{index}')
            .properties([
                f'vc-id = <{vc_id}>',
                f'port-index = <{port_index}>',
                f'bus-width = <{csi_lanes}>',
                'remote-endpoint = <&csi_in>',  # Should be updated later by `MediaGraph`
            ]))

        self._image_sensor = (DeviceTreeNode(f'imx728_{suffix}@{address}')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_imx728"',
                f'reg = <0x{address}>',
                'def-addr = <0x1a>',
                'mclk = "extperiph1"',
                'clocks = <&bpmp 36U>, <&bpmp 36U>',
                'clock-names = "extperiph1","pllp_grtba"',
                f'nvidia,isp-device = <&{self.isp.node_label}>',
                f'nvidia,gmsl-ser-device = <&{self.ser.node_label}>',
                f'nvidia,gmsl-dser-device = <&{dser_label}>',
                'physical_w = "15.0"',
                'physical_h = "12.5"',
                'sensor_model = "imx728"',
                'fsync-mode = "false"',
                'distortion-correction = "false"',
                'auto-exposure = "true"',
                'post_crop_frame_drop = "0"',
                'use_decibel_gain = "true"',
                'use_sensor_mode_id = "true"',
                f'reg_mux = <{int(index / 2)}>'
            ])
            .nodes([
                DeviceTreeNode('mode0')
                    .header('\n/* mode IMX728_MODE_3840X2160_CROP_20FPS */')
                    .properties([
                        'mclk_khz = "24000"',
                        f'num_lanes = "{csi_lanes}"',
                        'tegra_sinterface = "serial_a"',
                        f'vc_id = "{csi_lanes}"',
                        'discontinuous_clk = "no"',
                        'dpcm_enable = "false"',
                        'cil_settletime = "0"',
                        'dynamic_pixel_bit_depth = "16"',
                        'csi_pixel_bit_depth = "16"',
                        'mode_type = "yuv"',
                        'pixel_phase = "uyvy"',

                        'active_w = "3840"',
                        'active_h = "2160"',
                        # 'active_h = "2163"',
                        'readout_orientation = "0"',
                        'line_length = "4050"',
                        'inherent_gain = "1"',
                        'pix_clk_hz = "23328000"',
                        f'serdes_pix_clk_hz = "{int(serdes_pix_clk_hz)}"',

                        'gain_factor = "10"',
                        'min_gain_val = "0"',       # dB
                        'max_gain_val = "300"',     # dB
                        'step_gain_val = "3"',      # 0.3
                        'default_gain = "0"',
                        'min_hdr_ratio = "1"',
                        'max_hdr_ratio = "1"',
                        'framerate_factor = "1000000"',
                        'min_framerate = "20000000"',
                        'max_framerate = "20000000"',
                        'step_framerate = "1"',
                        'default_framerate = "20000000"',
                        'exposure_factor = "1000000"',
                        'min_exp_time = "24"',  # us 1 line
                        'max_exp_time = "33333"',
                        'step_exp_time = "1"',
                        'default_exp_time = "33333"',   # /* us */
                        'embedded_metadata_height = "0"',
                        # 'embedded_metadata_height = "3"',
                        # 'deskew_initial_enable = "true"',
                    ]),
                DeviceTreeNode('ports')
                    .properties([
                        '#address-cells = <1>',
                        '#size-cells = <0>',
                    ])
                    .nodes([
                        DeviceTreeNode('port@0')
                            .properties([
                                'reg = <0>',
                            ])
                            .nodes([self.endpoint])
                    ]),
                DeviceTreeNode('gmsl-link')
                    .properties([
                        'src-csi-port = "b"',
                        'dst-csi-port = "a"',
                        f'serdes-csi-link = "{serdes_csi_link}"',
                        f'csi-mode = "1x{csi_lanes}"',
                        f'st-vc = <{vc_id}>',
                        f'vc-id = <{vc_id}>',
                        f'num-lanes = <{csi_lanes}>',
                        'streams = "ued-u1","yuv8"',
                    ])
            ]))

    def to_list(self) -> List[Optional[DeviceTreeNode]]:
        return [self.ser_prim, self.ser, self.isp_prim, self.isp, self.image_sensor]
