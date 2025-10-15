#!/usr/bin/env python3

from typing import Optional, SupportsInt

from . import StringLike, validate_csi_lanes
from .devicetree import DeviceTreeNode, FragmentNode


class Serializer:
    @staticmethod
    def primary(index: int) -> DeviceTreeNode:
        return (DeviceTreeNode('max9295_prim@62', label=f'ser{index}_prim')
            .properties([
                'compatible = "nvidia,tier4_max9295"',
                'reg = <0x62>',
                'is-prim-ser',
                'status = "okay"',
            ]))

    @staticmethod
    def secondary_a(index: int, suffix: StringLike, vc_id: StringLike, dser_label: str) -> DeviceTreeNode:
        return (DeviceTreeNode(f'max9295_{suffix}@40', label=f'ser{index}_a')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_max9295"',
                'reg = <0x40>',
                f'nvidia,gmsl-dser-device = <&{dser_label}>',
                f'vc-id = <{vc_id}>',
            ]))

    @staticmethod
    def secondary_b(index: int, suffix: StringLike, vc_id: StringLike, dser_label: str) -> DeviceTreeNode:
        return (DeviceTreeNode(f'max9295_{suffix}@60', label=f'ser{index}_b')
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_max9295"',
                'reg = <0x60>',
                f'nvidia,gmsl-dser-device = <&{dser_label}>',
                f'vc-id = <{vc_id}>',
            ]))


class Deserializer:
    @staticmethod
    def node(name: str, label: str, csi_lanes: SupportsInt) -> DeviceTreeNode:
        csi_lanes = int(csi_lanes)
        validate_csi_lanes(csi_lanes)

        return (DeviceTreeNode(f'{name}@48', label=label)
            .properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_max9296"',
                'reg = <0x48>',
                f'csi-mode = "2x{csi_lanes}"',
                'max-src = <2>',
                'clk-mhz = <2000>',
            ]))

    @staticmethod
    def fragment(target: Optional[str] = None, target_path: Optional[str] = None,
                 csi_lanes: SupportsInt = -1) -> FragmentNode:
        csi_lanes = int(csi_lanes)
        validate_csi_lanes(csi_lanes)

        return FragmentNode(target=target, target_path=target_path) \
            .overlay_properties([
                'status = "okay"',
                'compatible = "nvidia,tier4_max9296"',
                'reg = <0x48>',
                f'csi-mode = "2x{csi_lanes}"',
                'max-src = <2>',
                'clk-mhz = <2000>',
            ])
