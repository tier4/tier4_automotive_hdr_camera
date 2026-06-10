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
TEGRA_CAMERA_BASE_OVERLAY_R36: str = 'dtsi/tegra-camera-base-r36.dtsi'

CSI_LANES: int = 4
NUM_GMSL_DESERIALIZERS: int = 4
TOTAL_CSI_LANES: int = CSI_LANES * NUM_GMSL_DESERIALIZERS

PIXEL_CLOCK: int = 625000000


def build_cameras(camera_list: Iterable[camera.Type], gmsl_dsers: Iterable[DeviceTreeNode]) -> List[camera.Camera]:
    def get_label(node: DeviceTreeNode) -> str:
        label = node.target_label if isinstance(node, FragmentNode) else node.node_label
        if label is None:
            raise ValueError('A GMSL deserializer node has no label')
        return label

    dser_label = list(filter(None, [get_label(gmsl_dser) for gmsl_dser in gmsl_dsers]))
    assert len(dser_label) >= 4

    c1_cameras = [
        camera.C1(index=7, suffix='a', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C1(index=6, suffix='b', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C1(index=5, suffix='c', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C1(index=4, suffix='d', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C1(index=3, suffix='e', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C1(index=2, suffix='f', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C1(index=1, suffix='g', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C1(index=0, suffix='h', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    c2_cameras = [
        camera.C2(index=7, suffix='a', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C2(index=6, suffix='b', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C2(index=5, suffix='c', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C2(index=4, suffix='d', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C2(index=3, suffix='e', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C2(index=2, suffix='f', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C2(index=1, suffix='g', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C2(index=0, suffix='h', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    c3_cameras = [
        camera.C3(index=7, suffix='a', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C3(index=6, suffix='b', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C3(index=5, suffix='c', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C3(index=4, suffix='d', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C3(index=3, suffix='e', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C3(index=2, suffix='f', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C3(index=1, suffix='g', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C3(index=0, suffix='h', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    c1mp_cameras = [
        camera.C1MP(index=7, suffix='a', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C1MP(index=6, suffix='b', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C1MP(index=5, suffix='c', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C1MP(index=4, suffix='d', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C1MP(index=3, suffix='e', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C1MP(index=2, suffix='f', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C1MP(index=1, suffix='g', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C1MP(index=0, suffix='h', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    c2mp_cameras = [
        camera.C2MP(index=7, suffix='a', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C2MP(index=6, suffix='b', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C2MP(index=5, suffix='c', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C2MP(index=4, suffix='d', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C2MP(index=3, suffix='e', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C2MP(index=2, suffix='f', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C2MP(index=1, suffix='g', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C2MP(index=0, suffix='h', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    c3mp_cameras = [
        camera.C3MP(index=7, suffix='a', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_a"),
        camera.C3MP(index=6, suffix='b', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_a"),
        camera.C3MP(index=5, suffix='c', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_c"),
        camera.C3MP(index=4, suffix='d', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_c"),
        camera.C3MP(index=3, suffix='e', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_e"),
        camera.C3MP(index=2, suffix='f', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_e"),
        camera.C3MP(index=1, suffix='g', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False, tegra_sinterface="serial_g"),
        camera.C3MP(index=0, suffix='h', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True,  tegra_sinterface="serial_g"),
    ]

    cameras_map: Dict[camera.Type, Sequence[camera.Camera]] = {
        camera.Type.C1: c1_cameras,
        camera.Type.C2: c2_cameras,
        camera.Type.C3: c3_cameras,
        camera.Type.C1MP: c1mp_cameras,
        camera.Type.C2MP: c2mp_cameras,
        camera.Type.C3MP: c3mp_cameras,
    }

    return [cameras_map[cam_type][i] for i, cam_type in enumerate(camera_list)]


T = TypeVar('T')


def at(lst: Sequence[T], index: int) -> Optional[T]:
    return lst[index] if index < len(lst) else None


def create_gmsl_dser_nodes(use_tier4mp_per_dser: Optional[List[bool]] = None) -> List[DeviceTreeNode]:
    if use_tier4mp_per_dser is None:
        use_tier4mp_per_dser = [False, False, False, False]

    dser_configs = [
        ('max9296_a', 'dsera', f'reset-gpios = <&tca9539_74 0 {const.GPIO_ACTIVE_LOW}>'),
        ('max9296_c', 'dserb', f'reset-gpios = <&tca9539_74 2 {const.GPIO_ACTIVE_LOW}>'),
        ('max9296_e', 'dserc', f'reset-gpios = <&tca9539_74 4 {const.GPIO_ACTIVE_LOW}>'),
        ('max9296_g', 'dserd', f'reset-gpios = <&tca9539_74 6 {const.GPIO_ACTIVE_LOW}>'),
    ]

    nodes = []
    for i, (name, label, reset_gpio) in enumerate(dser_configs):
        Dser = gmsl.Tier4MPDeserializer if use_tier4mp_per_dser[i] else gmsl.Deserializer
        nodes.append(
            Dser.node(name=name, csi_lanes=CSI_LANES, label=label)
                .properties([reset_gpio, 'fsync-gpi = <7>'])
        )
    return nodes


# Default tier4 deserializer nodes for backward compatibility
gmsl_dsers_nodes = create_gmsl_dser_nodes()


gmsl_dser_fragments = [
        gmsl.Deserializer.fragment(target='dsera', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 0 {const.GPIO_ACTIVE_HIGH}>',
                    'fsync-gpi = <7>'
                ]),
        gmsl.Deserializer.fragment(target='dserb', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 2 {const.GPIO_ACTIVE_HIGH}>',
                    'fsync-gpi = <7>'
                ]),
        gmsl.Deserializer.fragment(target='dserc', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 4 {const.GPIO_ACTIVE_HIGH}>',
                    'fsync-gpi = <7>'
                ]),
        gmsl.Deserializer.fragment(target='dserd', csi_lanes=CSI_LANES)
                .overlay_properties([
                    f'reset-gpios = <&tca9539_74 6 {const.GPIO_ACTIVE_HIGH}>',
                    'fsync-gpi = <7>'
                ]),
]


def generate_jetson_camera_overlay(opts: GeneratorOptions) -> DeviceTreeNode:
    num_channels = opts.number_of_cameras

    vi_node_fragment = tegra_vi.create_vi_node_fragment(opts) \
        .overlay_properties([f'num-channels = <{num_channels}>'])
    vi_ports = [
        ViPort(opts, port_number=7, port_index=0, vc_id=1, bus_width=CSI_LANES),
        ViPort(opts, port_number=6, port_index=0, vc_id=0, bus_width=CSI_LANES),
        ViPort(opts, port_number=5, port_index=2, vc_id=1, bus_width=CSI_LANES),
        ViPort(opts, port_number=4, port_index=2, vc_id=0, bus_width=CSI_LANES),
        ViPort(opts, port_number=3, port_index=4, vc_id=1, bus_width=CSI_LANES),
        ViPort(opts, port_number=2, port_index=4, vc_id=0, bus_width=CSI_LANES),
        ViPort(opts, port_number=1, port_index=5, vc_id=1, bus_width=CSI_LANES),
        ViPort(opts, port_number=0, port_index=5, vc_id=0, bus_width=CSI_LANES),
    ][:num_channels]
    vi_fragments = sum(map(ViPort.to_list, vi_ports), [vi_node_fragment])

    nvcsi_node_fragment = tegra_nvcsi.create_nvcsi_node_fragment(opts) \
        .overlay_properties([f'num-channels = <{num_channels}>'])
    nvcsi_channels = [
        NvcsiChannel(opts, channel_index=7, port_index=0, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=6, port_index=0, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=5, port_index=2, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=4, port_index=2, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=3, port_index=4, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=2, port_index=4, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=1, port_index=6, bus_width=CSI_LANES),
        NvcsiChannel(opts, channel_index=0, port_index=6, bus_width=CSI_LANES),
    ][:num_channels]
    nvcsi_fragments = sum(map(NvcsiChannel.to_list, nvcsi_channels), [nvcsi_node_fragment])

    root = devicetree.jetson_camera_overlay_root(opts.overlay_name)

    i2c_bus_number: int = 0x28
    i2c_mux_path = '/i2c@31e0000/tca9544@72'

    # Determine deserializer type per deserializer based on connected cameras
    # Each deserializer serves 2 cameras: dser[i] serves cameras[2*i] and cameras[2*i+1]
    use_tier4mp_per_dser = []
    for dser_idx in range(NUM_GMSL_DESERIALIZERS):
        # reverse the deserializer index to match the order of deserializer nodes in the devicetree
        reversed_dser_idx = (NUM_GMSL_DESERIALIZERS - 1) - dser_idx
        cam_idx_0 = reversed_dser_idx * 2
        cam_idx_1 = reversed_dser_idx * 2 + 1
        
        # Use tier4mp if any camera on this deserializer is tier4mp
        is_tier4mp = False
        if cam_idx_0 < len(opts.camera_list):
            is_tier4mp = is_tier4mp or opts.camera_list[cam_idx_0].is_tier4mp
        if cam_idx_1 < len(opts.camera_list):
            is_tier4mp = is_tier4mp or opts.camera_list[cam_idx_1].is_tier4mp
        use_tier4mp_per_dser.append(is_tier4mp)

    gmsl_dsers_nodes: Sequence[DeviceTreeNode] = create_gmsl_dser_nodes(use_tier4mp_per_dser=use_tier4mp_per_dser)

    if opts.l4t_version.major >= 36:
        i2c_mux_path = '/bus@0' + i2c_mux_path
        gmsl_dsers = gmsl_dsers_nodes
        with open(TEGRA_CAMERA_BASE_OVERLAY_R36, 'r', encoding='utf-8') as tegra_cam_dtsi:
            (root
                .header('\n' + tegra_cam_dtsi.read())
                .nodes(i2c_mux_and_gmsl_dsers(gmsl_dsers)))
    else:
        root.nodes(gmsl_dsers)

    cameras = build_cameras(opts.camera_list, gmsl_dsers)

    graph = MediaGraph(vi_ports, nvcsi_channels, cameras)
    graph.connect_all_endpoints()

    tegra_camera_platform_modules = [
        CameraPlatform.module_fragment(opts, index=7, cam=at(cameras, 0), position='bottom', i2c_bus_number=i2c_bus_number + 3, i2c_bus_path=f'{i2c_mux_path}/i2c@3'),
        CameraPlatform.module_fragment(opts, index=6, cam=at(cameras, 1), position='top',    i2c_bus_number=i2c_bus_number + 3, i2c_bus_path=f'{i2c_mux_path}/i2c@3'),
        CameraPlatform.module_fragment(opts, index=5, cam=at(cameras, 2), position='right2', i2c_bus_number=i2c_bus_number + 2, i2c_bus_path=f'{i2c_mux_path}/i2c@2'),
        CameraPlatform.module_fragment(opts, index=4, cam=at(cameras, 3), position='left2',  i2c_bus_number=i2c_bus_number + 2, i2c_bus_path=f'{i2c_mux_path}/i2c@2'),
        CameraPlatform.module_fragment(opts, index=3, cam=at(cameras, 4), position='right',  i2c_bus_number=i2c_bus_number + 1, i2c_bus_path=f'{i2c_mux_path}/i2c@1'),
        CameraPlatform.module_fragment(opts, index=2, cam=at(cameras, 5), position='left',   i2c_bus_number=i2c_bus_number + 1, i2c_bus_path=f'{i2c_mux_path}/i2c@1'),
        CameraPlatform.module_fragment(opts, index=1, cam=at(cameras, 6), position='front',  i2c_bus_number=i2c_bus_number + 0, i2c_bus_path=f'{i2c_mux_path}/i2c@0'),
        CameraPlatform.module_fragment(opts, index=0, cam=at(cameras, 7), position='rear',   i2c_bus_number=i2c_bus_number + 0, i2c_bus_path=f'{i2c_mux_path}/i2c@0'),
    ]
    tegra_camera_platform_fragments = sum(tegra_camera_platform_modules, [
        CameraPlatform.node_fragment(opts, num_csi_lanes=TOTAL_CSI_LANES, max_lane_speed=2500000)
    ])

    (root
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
                .overlay_nodes(sum(map(lambda cam: cam.to_list(), cameras[6:8]), []))
        ]))

    return root


def i2c_mux_and_gmsl_dsers(gmsl_dsers: Sequence[DeviceTreeNode]) -> List[FragmentNode]:
    return ([
        FragmentNode(target_path='/bus@0/i2c@31e0000')
            .overlay_nodes([
                DeviceTreeNode('tca9544@72')
                    .properties([
                        'status = "okay"',
                        'compatible = "nxp,pca9544"',
                        'reg = <0x72>',
                        '#address-cells = <1>',
                        '#size-cells = <0>',
                        'vcc-supply = <&vdd_1v8_sys>',
                        'vcc_lp = "vcc"',
                        'skip_mux_detect = "yes"',
                        'force_bus_start = <40>',
                    ])
                    .nodes([
                        DeviceTreeNode('i2c@0')
                            .properties([
                                'status = "okay"',
                                'reg = <0>',
                                'i2c-mux,deselect-on-exit',
                                '#address-cells = <1>',
                                '#size-cells = <0>',
                            ])
                            .nodes([
                                gmsl_dsers[0]
                            ]),
                        DeviceTreeNode('i2c@1')
                            .properties([
                                'status = "okay"',
                                'reg = <1>',
                                'i2c-mux,deselect-on-exit',
                                '#address-cells = <1>',
                                '#size-cells = <0>',
                            ])
                            .nodes([
                                gmsl_dsers[1]
                            ]),
                        DeviceTreeNode('i2c@2')
                            .properties([
                                'status = "okay"',
                                'reg = <2>',
                                'i2c-mux,deselect-on-exit',
                                '#address-cells = <1>',
                                '#size-cells = <0>',
                            ])
                            .nodes([
                                gmsl_dsers[2]
                            ]),
                        DeviceTreeNode('i2c@3')
                            .properties([
                                'status = "okay"',
                                'reg = <3>',
                                'i2c-mux,deselect-on-exit',
                                '#address-cells = <1>',
                                '#size-cells = <0>',
                            ])
                            .nodes([
                                gmsl_dsers[3]
                            ]),
                    ])
            ]),
            FragmentNode(target_path='/bus@0/i2c@3180000')
                .overlay_properties([
                    'status = "okay"',
                    '#address-cells = <1>',
                    '#size-cells = <0>',
                ])
                .overlay_nodes([
                    DeviceTreeNode('tca9539@74', label='tca9539_74')
                        .properties([
                            'compatible = "ti,tca9539"',
                            'gpio-controller',
                            '#gpio-cells = <2>',
                            'ngpios = <16>',
                            'reg = <0x74>',
                            'status = "okay"',
                        ])
                        .nodes([
                            DeviceTreeNode('tca9539_74_outlow')
                                .properties([
                                    'status = "okay"',
                                    'gpio-hog',
                                    'output-low',
                                ]),
                            DeviceTreeNode('tca9539_74_outhigh')
                                .properties(['status = "disabled"']),
                            DeviceTreeNode('tca9539_74_input')
                                .properties(['status = "disabled"']),
                        ])
                ])
        ])


if __name__ == '__main__':
    help_desc = 'The Jetson camera devicetree overlay generator for Connect Tech Anvil.'

    try:
        opts = GeneratorOptions.parse_args(description=help_desc)
    except ValueError as e:
        print(f'Error: {e}', file=sys.stderr)
        sys.exit(1)

    if not 35 <= opts.l4t_version.major <= 36:
        print(f'Error: {sys.argv[0]} only supports major version R35 and R36', file=sys.stderr)
        sys.exit(1)

    camera_overlay = generate_jetson_camera_overlay(opts)

    output_path = opts.get_overlay_output_path(PLATFORM_NAME)
    if output_path == '-':
        print(camera_overlay, end='')
    else:
        with open(output_path, 'w', encoding='utf-8') as overlay_dts_file:
            print(camera_overlay, end='', file=overlay_dts_file)
