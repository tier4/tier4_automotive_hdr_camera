#!/usr/bin/env python3

from typing import Any, Final, List, Optional, SupportsInt

from . import validate_csi_lanes
from .devicetree import FragmentNode

MAX_CHANNELS: Final[int] = 8

nvcsi_node_fragment = FragmentNode(target_path='/host1x@13e00000/nvcsi@15a00000') \
    .overlay_properties([
        'status = "okay"',
        f'num-channels = <{MAX_CHANNELS}>',
    ])


class NvcsiChannel:
    def __init__(self, channel_index: Any, port_index: Any, bus_width: SupportsInt) -> None:
        bus_width = int(bus_width)
        validate_csi_lanes(bus_width)

        channel_path = f'/host1x@13e00000/nvcsi@15a00000/channel@{channel_index}'

        self.channel = FragmentNode(target_path=channel_path) \
            .overlay_properties([
                'status = "okay"',
            ])

        self.port0 = FragmentNode(target_path=f'{channel_path}/ports/port@0') \
            .overlay_properties([
                'status = "okay"',
            ])

        self.port0_endpoint = FragmentNode(target_path=f'{channel_path}/ports/port@0/endpoint@{channel_index * 2}') \
            .overlay_target_label(f'csi_in{channel_index}') \
            .overlay_properties([
                'status = \"okay\"',
                f'port-index = <{port_index}>',
                f'bus-width = <{bus_width}>',
                'remote-endpoint = <&cam_out>',     # Should be updated later by `MediaGraph`
            ])

        self.port1 = FragmentNode(target_path=f'{channel_path}/ports/port@1') \
            .overlay_properties([
                'status = "okay"',
            ])

        self.port1_endpoint = FragmentNode(target_path=f'{channel_path}/ports/port@1/endpoint@{channel_index * 2 + 1}') \
            .overlay_target_label(f'csi_out{channel_index}') \
            .overlay_properties([
                'status = \"okay\"',
                f'port-index = <{port_index}>',
                f'bus-width = <{bus_width}>',
                'remote-endpoint = <&vi_in>',       # Should be updated later by `MediaGraph`
            ])

    def to_list(self) -> List[Optional[FragmentNode]]:
        return [self.channel, self.port0, self.port0_endpoint, self.port1, self.port1_endpoint]

    @property
    def sink_endpoint(self) -> FragmentNode:
        return self.port0_endpoint

    @property
    def source_endpoint(self) -> FragmentNode:
        return self.port1_endpoint
