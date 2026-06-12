#!/usr/bin/env python3
"""
Jetson camera devicetree overlay generator for ADLINK ROScube Orin.
"""

import sys
from typing import Dict, Iterable, List, Optional, Sequence, TypeVar, Any

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

PLATFORM_NAME: str = 'roscube-orin'

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

    # Helper to get power reference label
    def get_power_ref(idx):
        suffix = ['a', 'a', 'b', 'b', 'c', 'c', 'd', 'd'][idx]
        return f'max20089_{suffix}'

    # Create cameras using library classes
    c1_cameras = [
        camera.C1(index=0, suffix='a', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=1, suffix='b', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=2, suffix='c', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=3, suffix='d', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=4, suffix='e', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=5, suffix='f', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1(index=6, suffix='g', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1(index=7, suffix='h', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    # Apply patches to C1
    for i, cam in enumerate(c1_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

    c2_cameras = [
        camera.C2(index=0, suffix='a', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=1, suffix='b', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=2, suffix='c', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=3, suffix='d', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=4, suffix='e', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=5, suffix='f', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2(index=6, suffix='g', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2(index=7, suffix='h', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    for i, cam in enumerate(c2_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

    c3_cameras = [
        camera.C3(index=0, suffix='a', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=1, suffix='b', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=2, suffix='c', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=3, suffix='d', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=4, suffix='e', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=5, suffix='f', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3(index=6, suffix='g', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3(index=7, suffix='h', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    for i, cam in enumerate(c3_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

    c1mp_cameras = [
        camera.C1MP(index=0, suffix='a', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1MP(index=1, suffix='b', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1MP(index=2, suffix='c', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1MP(index=3, suffix='d', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1MP(index=4, suffix='e', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1MP(index=5, suffix='f', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C1MP(index=6, suffix='g', address='1b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C1MP(index=7, suffix='h', address='1c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    for i, cam in enumerate(c1mp_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

    c2mp_cameras = [
        camera.C2MP(index=0, suffix='a', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2MP(index=1, suffix='b', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2MP(index=2, suffix='c', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2MP(index=3, suffix='d', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2MP(index=4, suffix='e', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2MP(index=5, suffix='f', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C2MP(index=6, suffix='g', address='2b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C2MP(index=7, suffix='h', address='2c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    for i, cam in enumerate(c2mp_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

    c3mp_cameras = [
        camera.C3MP(index=0, suffix='a', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='a', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3MP(index=1, suffix='b', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=0, serdes_csi_link='b', dser_label=dser_label[0], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3MP(index=2, suffix='c', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='a', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3MP(index=3, suffix='d', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=2, serdes_csi_link='b', dser_label=dser_label[1], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3MP(index=4, suffix='e', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='a', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3MP(index=5, suffix='f', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=4, serdes_csi_link='b', dser_label=dser_label[2], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
        camera.C3MP(index=6, suffix='g', address='3b', vc_id='0', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='a', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=True),
        camera.C3MP(index=7, suffix='h', address='3c', vc_id='1', csi_lanes=CSI_LANES, port_index=6, serdes_csi_link='b', dser_label=dser_label[3], serdes_pix_clk_hz=PIXEL_CLOCK, primary=False),
    ]
    for i, cam in enumerate(c3mp_cameras):
        vc_id = '0' if i % 2 == 0 else '1'
        patch_camera_node(cam, i, get_power_ref(i), vc_id)

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
        ('max9296', 'dser_0'),
        ('max9296', 'dser_1'),
        ('max9296', 'dser_2'),
        ('max9296', 'dser_3'),
    ]

    nodes = []
    for i, (name, label) in enumerate(dser_configs):
        Dser = gmsl.Tier4MPDeserializer if use_tier4mp_per_dser[i] else gmsl.Deserializer
        
        # Create the node using library
        node = Dser.node(name=name, csi_lanes=CSI_LANES, label=label)
        
        node.properties([
            'clk-mhz = <2000>',
            'max-src = <2>',
            'fsync-gpi = <0>'
        ])
        
        nodes.append(node)
    return nodes


def generate_jetson_camera_overlay(opts: GeneratorOptions) -> DeviceTreeNode:
    num_channels = opts.number_of_cameras

    root = devicetree.jetson_camera_overlay_root(opts.overlay_name)
    root.properties([
        'compatible = "nvidia,p3737-0000+p3701-0000", "nvidia,tegra234", "nvidia,tegra23x"'
    ])

    # -------------------------------------------------------------
    # 1. VI Ports (Starts at Fragment @1)
    # -------------------------------------------------------------
    FragmentNode.next_index = 1
    
    vi_port_nodes = []
    for i in range(num_channels):
        if i < 2: vi_p_idx = 0
        elif i < 4: vi_p_idx = 2
        elif i < 6: vi_p_idx = 4
        else: vi_p_idx = 6 
        
        vc = 0 if i % 2 == 0 else 1
        node = create_vi_port_node(port_number=i, port_index=vi_p_idx, vc_id=vc, bus_width=CSI_LANES)
        vi_port_nodes.append(node)
        
    # NVCSI Channels using helper
    nvcsi_channel_nodes = []
    for i in range(num_channels):
        node = create_nvcsi_channel_node(channel_index=i)
        nvcsi_channel_nodes.append(node)

    i2c_bus_number: int = 9
    i2c_mux_path = '/bus@0/i2c@c250000/tca9546@70'

    use_tier4mp_per_dser = []
    for dser_idx in range(NUM_GMSL_DESERIALIZERS):
        cam_idx_0 = dser_idx * 2
        cam_idx_1 = dser_idx * 2 + 1
        is_tier4mp = False
        if cam_idx_0 < len(opts.camera_list):
            is_tier4mp = is_tier4mp or opts.camera_list[cam_idx_0].is_tier4mp
        if cam_idx_1 < len(opts.camera_list):
            is_tier4mp = is_tier4mp or opts.camera_list[cam_idx_1].is_tier4mp
        use_tier4mp_per_dser.append(is_tier4mp)

    gmsl_dsers: Sequence[DeviceTreeNode] = create_gmsl_dser_nodes(use_tier4mp_per_dser=use_tier4mp_per_dser)

    # Build cameras using local function that applies patches
    cameras = build_cameras(opts.camera_list, gmsl_dsers)

    # Manually link endpoints (VI <-> NVCSI <-> Camera)
    # 1. Link VI <-> NVCSI
    for i in range(num_channels):
        vi_ep = vi_port_nodes[i].child_nodes[0]
        # Reference links directly to csi_outX
        nvcsi_out_label = f'csi_out{i}'
        vi_ep.properties([f'remote-endpoint = <&{nvcsi_out_label}>'])

    # 2. Link Camera <-> NVCSI
    for i in range(len(cameras)):
        cam = cameras[i]
        if not cam: continue
        # Reference links directly to csi_inX
        nvcsi_in_label = f'csi_in{i}'
        cam.endpoint.properties([f'remote-endpoint = <&{nvcsi_in_label}>'])

    vi_fragments = [
        FragmentNode(target_path='/tegra-capture-vi')
            .overlay_properties([f'num-channels = <{num_channels}>', 'status = "okay"'])
            .overlay_nodes([
                DeviceTreeNode('ports')
                    .properties(['status = "okay"', '#address-cells = <1>', '#size-cells = <0>'])
                    .nodes(vi_port_nodes)
            ])
    ]

    # -------------------------------------------------------------
    # 2. NVCSI Definition (Starts at Fragment @19)
    # -------------------------------------------------------------
    FragmentNode.next_index = 19    
    nvcsi_fragments = [
        FragmentNode(target_path='/bus@0/host1x@13e00000/nvcsi@15a00000')
            .overlay_properties([
                f'num-channels = <{num_channels}>', 
                'status = "okay"',
                '#address-cells = <1>',
                '#size-cells = <0>'
            ])
            .overlay_nodes(nvcsi_channel_nodes)
    ]

    # -------------------------------------------------------------
    # 3. NVCSI Links (Starts at Fragment @20)
    # -------------------------------------------------------------
    FragmentNode.next_index = 20

    # Create Separate NVCSI Link Fragments
    nvcsi_link_fragments = create_nvcsi_link_fragments(num_channels, cameras)

    # -------------------------------------------------------------
    # 4. Redundant NVCSI & VI Enable (Starts at Fragment @55)
    # -------------------------------------------------------------
    FragmentNode.next_index = 55
    
    nvcsi_redundant_fragment = [
        FragmentNode(target_path='/bus@0/host1x@13e00000/nvcsi@15a00000')
                .overlay_properties(['status = "okay"', f'num-channels = <{num_channels}>'])
    ]
    
    # Add fragments to explicitly enable VI nodes (vi0, vi1)
    vi_enable_fragments = create_vi_enable_fragments()
    
    # -------------------------------------------------------------
    # 5. Platform (Starts at Fragment @70)
    # -------------------------------------------------------------
    FragmentNode.next_index = 70

    max_lane_speed = 4000000
    # Add max_pixel_rate to platform node
    platform_node = FragmentNode(target_path='/') \
                                .overlay_nodes([DeviceTreeNode('tegra-camera-platform') \
                                                .properties([
                                                    'compatible = "nvidia, tegra-camera-platform"',
                                                    'status = "okay"',
                                                    f'num_csi_lanes = <{TOTAL_CSI_LANES}>',
                                                    f'max_lane_speed = <{max_lane_speed}>',
                                                    'min_bits_per_pixel = <10>',
                                                    'vi_peak_byte_per_pixel = <2>',
                                                    'vi_bw_margin_pct = <25>',
                                                    'isp_peak_byte_per_pixel = <5>',
                                                    'isp_bw_margin_pct = <25>',
                                                    'max_pixel_rate = <160000>',
                                                    'modules {   \n'
	 				                    '                   module0 { drivernode0 { }; }; \n'
	 				                    '                   module1 { drivernode0 { }; }; \n'
	 				                    '                   module2 { drivernode0 { }; }; \n'
	 				                    '                   module3 { drivernode0 { }; }; \n'
	 				                    '                   module4 { drivernode0 { }; }; \n'
	 				                    '                   module5 { drivernode0 { }; }; \n'
	 				                    '                   module6 { drivernode0 { }; }; \n'
	 				                    '                   module7 { drivernode0 { }; }; \n'
                                        '                }\n'
                                                ])
                                            ])

    # -------------------------------------------------------------
    # 6. Modules (Starts at Fragment @71)
    # -------------------------------------------------------------
    FragmentNode.next_index = 71
    
    # Platform modules
    tegra_camera_platform_modules = []
>>>>>>> c2d5091 (dts_generator: add support for new L4T BSP versions and ECU platforms)
    for i in range(8):
        cam = at(cameras, i)
        frags = create_platform_module_fragment(
            index=i, 
            cam=cam, 
            position=['rear','front','topright','bottomright','topleft','centerright','centerleft','bottomleft'][i], 
            i2c_bus_number=i2c_bus_number + int(i/2), 
            i2c_bus_path=f'{i2c_mux_path}/i2c@{int(i/2)}'
        )
        tegra_camera_platform_modules.extend(frags)
    tegra_camera_platform_modules = [f for f in tegra_camera_platform_modules if f]

    tegra_camera_platform_fragments = [platform_node] + tegra_camera_platform_modules

    # -------------------------------------------------------------
    # 7. I2C Mux & Children (Starts at Fragment @90)
    # -------------------------------------------------------------
    FragmentNode.next_index = 90
    
    # Create I2C Mux fragments using the patched cameras
    i2c_mux_frags = create_i2c_mux_fragments(gmsl_dsers, i2c_bus_number, cameras)

    # -------------------------------------------------------------
    # 8. FPGA (Starts at Fragment @100)
    # -------------------------------------------------------------
    FragmentNode.next_index = 100
    
    fpga_fragment = [
        FragmentNode(target_path='/bus@0/i2c@3180000')
            .overlay_properties([
                '#address-cells = <1>',
                '#size-cells = <0>'
            ])
            .overlay_nodes([
                DeviceTreeNode('tier4_fpga@66', label='t4_fpga')
                    .properties([
                        'compatible = "nvidia,tier4_fpga"',
                        'reg = <0x66>',
                        'status = "okay"',
                    ])
        ])
    ]

    # Assemble the root node
    (root
        .nodes(vi_fragments)
        .nodes(nvcsi_fragments)
        .nodes(nvcsi_link_fragments) 
        .nodes(nvcsi_redundant_fragment)
        .nodes(vi_enable_fragments)
        .nodes(tegra_camera_platform_fragments)
        .nodes(i2c_mux_frags)
        .nodes(fpga_fragment)
    )

    return root


def create_nvcsi_channel_node(channel_index: int) -> DeviceTreeNode:
    """
    Create NVCSI channel structure matching the reference DTS.
    Endpoints are endpoint@0 and endpoint@1 with local reg 0/1.
    """
    return DeviceTreeNode(f'channel@{channel_index}') \
        .properties([
            'status = "okay"',
            f'reg = <{channel_index}>',
        ]) \
        .nodes([
            DeviceTreeNode('ports')
                .properties([
                    '#address-cells = <1>',
                    '#size-cells = <0>',
                    'status = "okay"',
                ])
                .nodes([
                    DeviceTreeNode('port@0')
                        .properties([
                            'reg = <0>', 
                            'status = "okay"',
                        ])
                        .nodes([
                            DeviceTreeNode('endpoint@0')
                                .properties(['status = "okay"'])
                                .label(f'csi_in{channel_index}')
                        ]),
                    DeviceTreeNode('port@1')
                        .properties([
                            'reg = <1>', 
                            'status = "okay"',
                        ])
                        .nodes([
                            DeviceTreeNode('endpoint@1')
                                .properties(['status = "okay"'])
                                .label(f'csi_out{channel_index}')
                        ])
                ])
        ])

def create_nvcsi_link_fragments(num_channels: int, cameras: List[camera.Camera]) -> List[FragmentNode]:
    """
    Create separate fragments for NVCSI linking and configuration, 
    matching fragments 20-54 in the reference DTS.
        """
    fragments = []
    base_path = "/bus@0/host1x@13e00000/nvcsi@15a00000"

    for i in range(num_channels):
        channel_path = f"{base_path}/channel@{i}"
        
        # Skip channel status for Ch 6, 7
        if i not in [6, 7]:
            fragments.append(FragmentNode(target_path=channel_path).overlay_properties(['status = "okay"']))
        
        # Skip port0 status for Ch 6, 7
        if i not in [6, 7]:
            fragments.append(FragmentNode(target_path=f"{channel_path}/ports/port@0").overlay_properties(['status = "okay"']))
        
        # Endpoint 0 (Input) Configuration
        port_index = (i // 2) * 2
        cam = cameras[i] if i < len(cameras) else None
        cam_label = cam.endpoint.node_label if cam else f'cam_out{i}'
        
        fragments.append(
            FragmentNode(target_path=f"{channel_path}/ports/port@0/endpoint@0")
            .overlay_properties([
                'status = "okay"',
                f'port-index = <{port_index}>',
                f'bus-width = <{CSI_LANES}>',
                f'remote-endpoint = <&{cam_label}>'
            ])
        )
        
        # Skip port1 status for Ch 1
        if i != 1:
            fragments.append(FragmentNode(target_path=f"{channel_path}/ports/port@1").overlay_properties(['status = "okay"']))
        
        # Endpoint 1 (Output) Configuration
        fragments.append(
            FragmentNode(target_path=f"{channel_path}/ports/port@1/endpoint@1")
            .overlay_properties([
                'status = "okay"',
                f'remote-endpoint = <&vi_in{i}>'
            ])
        )

    return fragments

def create_vi_port_node(port_number: int, port_index: int, vc_id: int, bus_width: int) -> DeviceTreeNode:
    """Fix: ViPort.port_node() is missing in library."""
    return DeviceTreeNode(f'port@{port_number}') \
        .properties([
            'status = "okay"',
            f'reg = <{port_number}>',
        ]) \
        .nodes([
            DeviceTreeNode('endpoint')
                .properties([
                    'status = "okay"',
                    f'port-index = <{port_index}>' if port_index!=6 else f'port-index = <5>',
                    f'vc-id = <{vc_id}>',
                    f'bus-width = <{bus_width}>',
                ])
                .label(f'vi_in{port_number}')
        ])

def create_platform_module_fragment(index: int, cam: Optional[camera.Camera], position: str,
                                    i2c_bus_number: int, i2c_bus_path: str) -> List[Optional[FragmentNode]]:
    """Fix: Incorrect drivernode property (proc-device-tree -> sysfs-device-tree) and path."""
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

    # Use sysfs-device-tree and correct path prefix
    module0_drivernode0 = FragmentNode(target_path=f'{module_path}/drivernode0') \
        .overlay_properties([
            'status = "okay"',
            'pcl_id = "v4l2_sensor"',
            f'devname = "{image_sensor_name} {i2c_bus_number}-00{cam.sensor_address}"',
            f'sysfs-device-tree = "/sys/firmware/devicetree/base{i2c_bus_path}/{cam.image_sensor.node_name}"',
        ])

    return [module0, module0_drivernode0]

def patch_camera_node(cam: camera.Camera, index: int, power_ref: str, correct_vc_id: str):
    """
    Patch the Camera object to add missing properties, fix vc_id, AND fix tegra_sinterface.
    """
    if not cam:
        return
    
    sensor_node = cam.image_sensor
    
    # 1. Add missing properties (FPGA & Power)
    sensor_node.properties([
        'nvidia,fpga-device  = <&t4_fpga>',
        f'nvidia,cam-power-protector = <&{power_ref}>'
    ])

    # Determine correct tegra_sinterface based on camera index
    sinterface_map = ['serial_a', 'serial_a', 'serial_c', 'serial_c', 
                      'serial_e', 'serial_e', 'serial_g', 'serial_g']
    correct_sinterface = sinterface_map[index]

    # 2. Fix vc_id AND tegra_sinterface in mode nodes
    for child in sensor_node.child_nodes:
        if child.node_name.startswith('mode'):
            child.properties([
                f'vc_id = "{correct_vc_id}"',
                f'tegra_sinterface = "{correct_sinterface}"'
            ])
    
    # 3. Patch Serializer VC-ID
    try:
        if hasattr(cam, 'ser') and cam.ser:
            cam.ser.properties([f'vc-id = <{correct_vc_id}>'])
    except:
        pass

def create_vi_enable_fragments() -> List[FragmentNode]:
    """Create fragments to explicitly enable vi0 and vi1 nodes as seen in OK DTS."""
    # This will be fragments 56, 57
    return [
        FragmentNode(target_path="/bus@0/host1x@13e00000/vi1@14c00000")
            .overlay_properties(['status = "okay"']),
        FragmentNode(target_path="/bus@0/host1x@13e00000/vi0@15c00000")
            .overlay_properties(['status = "okay"'])
    ]

def create_i2c_mux_fragments(gmsl_dsers: Sequence[DeviceTreeNode], i2c_bus_number: int, cameras: List[camera.Camera]) -> List[FragmentNode]:
    # Use c250000 (i2c-7) as confirmed by OK logs
    I2C_BUS_ADDR = "c250000" 
    
    # 1. Define Mux (Fragment 90)
    mux_fragment = FragmentNode(target_path=f'/bus@0/i2c@{I2C_BUS_ADDR}') \
        .overlay_properties([
            '#address-cells = <1>',
            '#size-cells = <0>'
        ]) \
        .overlay_nodes([
            DeviceTreeNode('tca9546@70')
                .properties([
                    'status = "okay"',
                    '#address-cells = <1>',
                    '#size-cells = <0>',
                    'compatible = "nxp,pca9546"',
                    'reg = <0x70>',
                    'skip_mux_detect = "yes"',
                    f'force_bus_start = <{hex(i2c_bus_number)}>',
                ])
                .nodes([
                    DeviceTreeNode('i2c@0').properties(['reg = <0>', 'status = "okay"']),
                    DeviceTreeNode('i2c@1').properties(['reg = <1>', 'status = "okay"']),
                    DeviceTreeNode('i2c@2').properties(['reg = <2>', 'status = "okay"']),
                    DeviceTreeNode('i2c@3').properties(['reg = <3>', 'status = "okay"']),
                ])
        ])

    child_fragments = []
    
    def create_max20089(bus_idx):
        addr = 0x28 if bus_idx < 2 else 0x29
        label_char = chr(ord('a') + bus_idx)
        return DeviceTreeNode(f'max20089@{addr:x}', label=f'max20089_{label_char}') \
            .properties([
                'status = "okay"',
                'compatible = "maxim,max20089"',
                f'reg = <0x{addr:x}>',
            ])

    for i in range(4):
        # Flatten the list of camera nodes for this segment
        segment_cameras = sum(map(lambda cam: cam.to_list(), cameras[i*2 : i*2+2]), [])
        
        # Fragments 91, 92, 93, 94
        frag = FragmentNode(target_path=f'/bus@0/i2c@{I2C_BUS_ADDR}/tca9546@70/i2c@{i}') \
            .overlay_properties([
                'i2c-mux,deselect-on-exit',
                '#address-cells = <1>',
                '#size-cells = <0>',
            ]) \
            .overlay_nodes([
                gmsl_dsers[i],
                create_max20089(i)
            ] + segment_cameras)
        child_fragments.append(frag)

    return [mux_fragment] + child_fragments

# ----------------------------------------------

if __name__ == '__main__':
    help_desc = 'The Jetson camera devicetree overlay generator for ADLINK ROScube Orin.'

    try:
        opts = GeneratorOptions.parse_args(description=help_desc)
    except ValueError as e:
        print(f'Error: {e}', file=sys.stderr)
        sys.exit(1)

    if opts.l4t_version.major != 36:
        print(f'Error: {sys.argv[0]} only supports R36', file=sys.stderr)
        sys.exit(1)

    camera_overlay = generate_jetson_camera_overlay(opts)

    output_path = opts.get_overlay_output_path(PLATFORM_NAME)
    if output_path == '-':
        print(camera_overlay, end='')
    else:
        with open(output_path, 'w', encoding='utf-8') as overlay_dts_file:
            print(camera_overlay, end='', file=overlay_dts_file)
