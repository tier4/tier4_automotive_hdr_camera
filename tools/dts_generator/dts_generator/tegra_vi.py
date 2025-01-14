#!/usr/bin/env python3

from typing import List, Optional, SupportsInt

from . import StringLike, camera, validate_csi_lanes
from .devicetree import FragmentNode
from .options import GeneratorOptions
from .tegra_nvcsi import MAX_CHANNELS


def create_vi_node_fragment(opts: GeneratorOptions, index: Optional[int] = None) -> FragmentNode:
    return FragmentNode(index=index, target_path='/tegra-capture-vi') \
        .overlay_properties([
            'status = "okay"',
            f'num-channels = <{MAX_CHANNELS}>',
        ])


class ViPort:
    def __init__(self, opts: GeneratorOptions, port_number: StringLike, port_index: StringLike, vc_id: StringLike, bus_width: SupportsInt) -> None:
        bus_width = int(bus_width)
        validate_csi_lanes(bus_width)

        vi_port_path = f'/tegra-capture-vi/ports/port@{port_number}'

        self.port = FragmentNode(index=None, target_path=vi_port_path) \
            .overlay_properties([
                'status = "okay"',
            ])

        self.endpoint = FragmentNode(target_path=f'{vi_port_path}/endpoint') \
            .overlay_target_label(f'vi_in{port_number}') \
            .overlay_properties([
                'status = "okay"',
                f'port-index = <{port_index}>',
                f'vc-id = <{vc_id}>',
                f'bus-width = <{bus_width}>',
                'remote-endpoint = <&csi_out>',    # Should be updated later by `MediaGraph`
            ])

    def to_list(self) -> List[Optional[FragmentNode]]:
        return [self.port, self.endpoint]


class CameraPlatform:
    @staticmethod
    def node_fragment(opts: GeneratorOptions, num_csi_lanes: StringLike, max_lane_speed: StringLike) -> FragmentNode:
        return FragmentNode(target_path='/tegra-camera-platform') \
            .overlay_properties([
                'status = "okay"',
                f'num_csi_lanes = <{num_csi_lanes}>',
                f'max_lane_speed = <{max_lane_speed}>',
                'min_bits_per_pixel = <10>',
                'vi_peak_byte_per_pixel = <2>',
                'vi_bw_margin_pct = <25>',
                'isp_peak_byte_per_pixel = <5>',
                'isp_bw_margin_pct = <25>',
                # '//max_pixel_rate = <750000>',
            ])

    # The `position` argument corresponds to the `position` property for `tegra-camera-platform`.
    # It seems like only Argus makes use of it.
    @staticmethod
    def module_fragment(opts: GeneratorOptions, index: int, cam: Optional[camera.Camera], position: str,
                        i2c_bus_number: StringLike, i2c_bus_path: StringLike
                        ) -> List[Optional[FragmentNode]]:
        if cam is None:
            return [None]

        image_sensor_name = cam.camera_type().image_sensor_name

        module_path = f'/tegra-camera-platform/modules/module{index}'
        module0 = FragmentNode(target_path=module_path) \
            .overlay_properties([
                f'badge = "{image_sensor_name}_{position}"',
                f'position = "{position}"',
                'orientation = "1"',
                'status = "okay"',
            ])

        module0_drivernode0 = FragmentNode(target_path=f'{module_path}/drivernode0') \
            .overlay_properties([
                'status = "okay"',
                'pcl_id = "v4l2_sensor"',
                f'devname = "{image_sensor_name} {i2c_bus_number}-00{cam.sensor_address}"',
                f'proc-device-tree = "/proc/device-tree{i2c_bus_path}/{cam.image_sensor.node_name}"',
            ])

        return [module0, module0_drivernode0]
