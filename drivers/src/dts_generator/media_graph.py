#!/usr/bin/env python3

from dataclasses import dataclass
from typing import List

from .camera import Camera
from .tegra_nvcsi import NvcsiChannel
from .tegra_vi import ViPort


@dataclass
class MediaGraph:
    vi_ports: List[ViPort]
    nvcsi_channels: List[NvcsiChannel]
    cameras: List[Camera]

    def connect_all_endpoints(self) -> None:
        for i in range(0, len(self.vi_ports)):
            MediaGraph.connect_endpoints(self.vi_ports[i], self.nvcsi_channels[i], self.cameras[i])

    @staticmethod
    def connect_endpoints(vi_port: ViPort, nvcsi_channel: NvcsiChannel, camera: Camera) -> None:
        vi_port.endpoint.overlay_properties([
            'remote-endpoint = <&%s>' % nvcsi_channel.source_endpoint.target_label
        ])
        nvcsi_channel.source_endpoint.overlay_properties([
            'remote-endpoint = <&%s>' % vi_port.endpoint.target_label
        ])

        nvcsi_channel.sink_endpoint.overlay_properties([
            'remote-endpoint = <&%s>' % camera.endpoint.node_label
        ])
        camera.endpoint.properties([
            'remote-endpoint = <&%s>' % nvcsi_channel.sink_endpoint.target_label
        ])
