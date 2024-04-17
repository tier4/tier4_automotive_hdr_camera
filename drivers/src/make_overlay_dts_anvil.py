#!/usr/bin/env python3
"""
Jetson camera devicetree overlay generator for CTI Anvil.

* Devicetrees that this program generates are based on the devicetree CTI provides

    * It defines camera nodes in reverse order
    * 43-003c, the 8th camera node on the 4th (out of 4) i2c mux bus corresponds to the camera connected to the GMSL port 1
"""

import sys
from typing import Dict, Iterable, List, Optional, Sequence, TypeVar

from dts_generator import camera
from dts_generator import const
from dts_generator import devicetree
from dts_generator import gmsl
from dts_generator import tegra_nvcsi
from dts_generator import tegra_vi
from dts_generator.devicetree import DeviceTreeNode, FragmentNode
from dts_generator.media_graph import MediaGraph
from dts_generator.options import GeneratorOptions
from dts_generator.tegra_nvcsi import NvcsiChannel
from dts_generator.tegra_vi import CameraPlatform, ViPort

PLATFORM_NAME: str = 'Anvil'

CSI_LANES: int = 4
NUM_GMSL_DESERIALIZERS: int = 4
TOTAL_CSI_LANES: int = CSI_LANES * NUM_GMSL_DESERIALIZERS

PIXEL_CLOCK: int = 350000000


def build_cameras(camera_list: Iterable[camera.Type], gmsl_dsers: Iterable[FragmentNode]) -> List[camera.Camera]:
    dser_label = list(filter(None, [gmsl_dser.target_label for gmsl_dser in gmsl_dsers]))
    assert len(dser_label) >= 4

    c1_cameras = [
        camera.C1(index=7, suffix='a', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=6, suffix='b', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=5, suffix='c', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=4, suffix='d', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=3, suffix='e', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=2, suffix='f', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=1, suffix='g', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=0, suffix='h', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
    ]

    c2_cameras = [
        camera.C2(index=7, suffix='a', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=6, suffix='b', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=5, suffix='c', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=4, suffix='d', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=3, suffix='e', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=2, suffix='f', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=1, suffix='g', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=0, suffix='h', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
    ]

    c3_cameras = [
        camera.C3(index=7, suffix='a', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=6, suffix='b', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=5, suffix='c', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=4, suffix='d', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=3, suffix='e', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=2, suffix='f', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=1, suffix='g', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=0, suffix='h', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
    ]

    cameras_map: Dict[camera.Type, Sequence[camera.Camera]] = {
        camera.Type.C1: c1_cameras,
        camera.Type.C2: c2_cameras,
        camera.Type.C3: c3_cameras,
    }

    return [cameras_map[cam_type][i] for i, cam_type in enumerate(camera_list)]


T = TypeVar('T')


def at(lst: Sequence[T], index: int) -> Optional[T]:
    return lst[index] if index < len(lst) else None


def generate_jetson_camera_overlay(opts: GeneratorOptions) -> DeviceTreeNode:
    num_channels = opts.number_of_cameras

    tegra_vi.vi_node_fragment.overlay_properties([f'num-channels = <{num_channels}>'])
    vi_ports = [
        ViPort(port_number=7, port_index=0, vc_id=1, bus_width=CSI_LANES),
        ViPort(port_number=6, port_index=0, vc_id=0, bus_width=CSI_LANES),
        ViPort(port_number=5, port_index=2, vc_id=1, bus_width=CSI_LANES),
        ViPort(port_number=4, port_index=2, vc_id=0, bus_width=CSI_LANES),
        ViPort(port_number=3, port_index=4, vc_id=1, bus_width=CSI_LANES),
        ViPort(port_number=2, port_index=4, vc_id=0, bus_width=CSI_LANES),
        ViPort(port_number=1, port_index=5, vc_id=1, bus_width=CSI_LANES),
        ViPort(port_number=0, port_index=5, vc_id=0, bus_width=CSI_LANES),
    ][:num_channels]
    vi_fragments = sum(map(ViPort.to_list, vi_ports), [tegra_vi.vi_node_fragment])

    tegra_nvcsi.nvcsi_node_fragment.overlay_properties([f'num-channels = <{num_channels}>'])
    nvcsi_channels = [
        NvcsiChannel(channel_index=7, port_index=0, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=6, port_index=0, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=5, port_index=2, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=4, port_index=2, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=3, port_index=4, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=2, port_index=4, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=1, port_index=6, bus_width=CSI_LANES),
        NvcsiChannel(channel_index=0, port_index=6, bus_width=CSI_LANES),
    ][:num_channels]
    nvcsi_fragments = sum(map(NvcsiChannel.to_list, nvcsi_channels), [tegra_nvcsi.nvcsi_node_fragment])

    gmsl_dsers = [
        gmsl.Deserializer.fragment(target='dsera', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 0 {const.GPIO_ACTIVE_HIGH}>'
                ]),
        gmsl.Deserializer.fragment(target='dserb', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 2 {const.GPIO_ACTIVE_HIGH}>'
                ]),
        gmsl.Deserializer.fragment(target='dserc', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 4 {const.GPIO_ACTIVE_HIGH}>'
                ]),
        gmsl.Deserializer.fragment(target='dserd', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 6 {const.GPIO_ACTIVE_HIGH}>'
                ]),
    ]
    cameras = build_cameras(opts.camera_list, gmsl_dsers)

    graph = MediaGraph(vi_ports, nvcsi_channels, cameras)
    graph.connect_all_endpoints()

    i2c_bus_number: int = 0x28
    i2c_mux_path = '/i2c@31e0000/tca9544@72'

    tegra_camera_platform_modules = [
        CameraPlatform.module_fragment(index=7, cam=at(cameras, 0), position='bottom', i2c_bus_number=i2c_bus_number + 3, i2c_bus_path=f'{i2c_mux_path}/i2c@3'),
        CameraPlatform.module_fragment(index=6, cam=at(cameras, 1), position='top',    i2c_bus_number=i2c_bus_number + 3, i2c_bus_path=f'{i2c_mux_path}/i2c@3'),
        CameraPlatform.module_fragment(index=5, cam=at(cameras, 2), position='right2', i2c_bus_number=i2c_bus_number + 2, i2c_bus_path=f'{i2c_mux_path}/i2c@2'),
        CameraPlatform.module_fragment(index=4, cam=at(cameras, 3), position='left2',  i2c_bus_number=i2c_bus_number + 2, i2c_bus_path=f'{i2c_mux_path}/i2c@2'),
        CameraPlatform.module_fragment(index=3, cam=at(cameras, 4), position='right',  i2c_bus_number=i2c_bus_number + 1, i2c_bus_path=f'{i2c_mux_path}/i2c@1'),
        CameraPlatform.module_fragment(index=2, cam=at(cameras, 5), position='left',   i2c_bus_number=i2c_bus_number + 1, i2c_bus_path=f'{i2c_mux_path}/i2c@1'),
        CameraPlatform.module_fragment(index=1, cam=at(cameras, 6), position='front',  i2c_bus_number=i2c_bus_number + 0, i2c_bus_path=f'{i2c_mux_path}/i2c@0'),
        CameraPlatform.module_fragment(index=0, cam=at(cameras, 7), position='rear',   i2c_bus_number=i2c_bus_number + 0, i2c_bus_path=f'{i2c_mux_path}/i2c@0'),
    ]
    tegra_camera_platform_fragments = sum(tegra_camera_platform_modules, [
        CameraPlatform.node_fragment(num_csi_lanes=TOTAL_CSI_LANES, max_lane_speed=2500000)
    ])

    root = (devicetree.jetson_camera_overlay_root(opts.overlay_name)
        .nodes(vi_fragments)
        .nodes(nvcsi_fragments)
        .nodes(tegra_camera_platform_fragments)
        .nodes([
            FragmentNode(target_path='/')
                .overlay_properties([
                    # string list of all the connected cameras
                    'tier4,cameras = %s' % devicetree.string_list([cam.name for cam in opts.camera_list]),
                ]),

            FragmentNode(target_path=i2c_mux_path)
                .overlay_properties([
                    'status = "okay"',
                    'compatible = "nxp,pca9544"',
                    'reg = <0x72>',
                    '#address-cells = <1>',
                    '#size-cells = <0>',
                    'vcc-supply = <&vdd_1v8_sys>',
                    'skip_mux_detect',
                    f'force_bus_start = <{i2c_bus_number}>',
                ]),
            FragmentNode(target_path=f'{i2c_mux_path}/i2c@3')
                .overlay_properties([
                    'status = "okay"',
                    'reg = <3>',
                    'i2c-mux,deselect-on-exit',
                ])
                .overlay_nodes(sum(map(lambda cam: cam.to_list(), cameras[0:2]), [])),
            FragmentNode(target_path=f'{i2c_mux_path}/i2c@2')
                .overlay_properties([
                    'status = "okay"',
                    'reg = <2>',
                    'i2c-mux,deselect-on-exit',
                ])
                .overlay_nodes(sum(map(lambda cam: cam.to_list(), cameras[2:4]), [])),
            FragmentNode(target_path=f'{i2c_mux_path}/i2c@1')
                .overlay_properties([
                    'status = "okay"',
                    'reg = <1>',
                    'i2c-mux,deselect-on-exit',
                ])
                .overlay_nodes(sum(map(lambda cam: cam.to_list(), cameras[4:6]), [])),
            FragmentNode(target_path=f'{i2c_mux_path}/i2c@0')
                .overlay_properties([
                    'status = "okay"',
                    'reg = <0>',
                    'i2c-mux,deselect-on-exit',
                ])
                .overlay_nodes(sum(map(lambda cam: cam.to_list(), cameras[6:8]), [])),
        ])
        .nodes(gmsl_dsers))

    return root


if __name__ == '__main__':
    help_desc = 'The Jetson camera devicetree overlay generator for Connect Tech Anvil.'

    try:
        opts = GeneratorOptions.parse_args(description=help_desc)
    except ValueError as e:
        print(f'Error: {e}', file=sys.stderr)
        sys.exit(1)

    if opts.l4t_version.major != 35:
        print(f'Error: {sys.argv[0]} only supports major version R35', file=sys.stderr)
        sys.exit(1)

    camera_overlay = generate_jetson_camera_overlay(opts)

    output_path = opts.get_overlay_output_path(PLATFORM_NAME)
    if output_path == '-':
        print(camera_overlay, end='')
    else:
        with open(output_path, 'w', encoding='utf-8') as overlay_dts_file:
            print(camera_overlay, end='', file=overlay_dts_file)
