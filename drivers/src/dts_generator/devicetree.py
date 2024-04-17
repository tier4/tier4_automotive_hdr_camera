#!/usr/bin/env python3

from __future__ import annotations

import copy
from functools import reduce
from typing import Dict, Final, Iterable, List, Optional, Tuple


def _indent_lines(lines: str, depth: int) -> str:
    return ''.join(map(lambda s: ('\t' * depth) + s, lines.splitlines(keepends=True)))


def _split_property(property_str: str) -> Tuple[str, Optional[str]]:
    pair = property_str.strip().split('=', maxsplit=1)
    return (pair[0].strip(), pair[1].strip()) if len(pair) == 2 else (pair[0], None)


class PropertyMap:
    def __init__(self) -> None:
        self.properties: Dict[str, Optional[str]] = {}

    def update(self, props: Iterable[str]) -> None:
        prop_pairs = [_split_property(prop) for prop in props]
        self.properties.update(prop_pairs)

    def remove(self, property_names: Iterable[str]) -> None:
        for prop in property_names:
            try:
                del self.properties[prop]
            except KeyError:
                pass

    def to_dts(self, depth: int = 0) -> str:
        return ''.join(map(lambda prop:
                           ('\t' * depth) + prop[0] + ('' if prop[1] is None else (' = ' + prop[1])) + ';\n',
                           self.properties.items()))

    def __str__(self) -> str:
        return self.to_dts(depth=0)


class DeviceTreeNode:
    def __init__(self, name: str, label: str = '') -> None:
        self.header_str: str = ''
        self.node_name: str = name
        self.node_label: str = label
        self.props = PropertyMap()
        self.child_nodes: List[DeviceTreeNode] = []
        self.footer_str: str = ''

    def header(self, header: str) -> DeviceTreeNode:
        self.header_str = header
        return self

    def name(self, name: str) -> DeviceTreeNode:
        self.node_name = name
        return self

    def label(self, label: str) -> DeviceTreeNode:
        self.node_label = label
        return self

    def properties(self, props: Iterable[str]) -> DeviceTreeNode:
        self.props.update(props)
        return self

    def remove_properties(self, props: Iterable[str]) -> DeviceTreeNode:
        self.props.remove(props)
        return self

    def nodes(self, nodes: Iterable[Optional[DeviceTreeNode]]) -> DeviceTreeNode:
        node_list = [node for node in nodes if node is not None]
        self.child_nodes.extend(node_list)
        return self

    def footer(self, footer: str) -> DeviceTreeNode:
        self.footer_str = footer
        return self

    def to_dts(self, depth: int = 0) -> str:
        rep = _indent_lines(self.header_str, depth) + '\n'

        rep += _indent_lines(('' if self.node_label == '' else self.node_label + ': ') + self.node_name, depth) + ' {\n'
        rep += self.props.to_dts(depth=depth + 1)
        rep += ''.join(map(lambda node: node.to_dts(depth=depth + 1), self.child_nodes))
        rep += _indent_lines('};', depth) + '\n'

        rep += _indent_lines(self.footer_str, depth)
        return rep

    def __str__(self) -> str:
        return self.to_dts(depth=0)


# The fragment variant of DeviceTreeNode with:
#  - managed fragment indexing
#  - convenient accessors to the child '__overlay__' node
class FragmentNode(DeviceTreeNode):
    next_index: int = 1

    @classmethod
    def get_next_index(cls) -> int:
        index = cls.next_index
        cls.next_index += 1
        return index

    def __init__(self, target: Optional[str] = None, target_path: Optional[str] = None,
                 index: Optional[int] = None, label: str = '') -> None:
        if (target is None and target_path is None) or (target is not None and target_path is not None):
            raise ValueError('FragmentNode: either target or target_path must be valid')

        if index is None:
            index = FragmentNode.get_next_index()

        super().__init__(f'fragment@{index}', label=label)
        self.target_label = target
        self.overlay_node = DeviceTreeNode('__overlay__')
        self.nodes([self.overlay_node])

        if target is not None:
            self.properties([
                f'target = <&{target}>',
            ])

        if target_path is not None:
            self.properties([
                f'target-path = "{target_path}"',
            ])

    # This method deepcopies itself with the new fragment index
    def index(self, index: Optional[int]) -> FragmentNode:
        if index is None:
            index = FragmentNode.get_next_index()

        new_fragment = copy.deepcopy(self)
        new_fragment.name(f'fragment@{index}')
        return new_fragment

    def target_path(self, target_path: str) -> FragmentNode:
        self.properties([f'target-path = "{target_path}"'])
        return self

    def overlay_properties(self, props: Iterable[str]) -> FragmentNode:
        self.overlay_node.properties(props)
        return self

    def overlay_remove_properties(self, props: Iterable[str]) -> FragmentNode:
        self.overlay_node.remove_properties(props)
        return self

    def overlay_nodes(self, nodes: Iterable[Optional[DeviceTreeNode]]) -> FragmentNode:
        self.overlay_node.nodes(nodes)
        return self

    def overlay_target_label(self, label: str) -> FragmentNode:
        self.target_label = label
        return self


def quote(string: str) -> str:
    """Quote a value of devicetree 'string' type"""
    return f'"{string}"'


def unquote(string: str) -> str:
    """Unquote a value of devicetree 'string' type"""
    return string.strip('"')


def string_list(strings: Iterable[str]) -> str:
    return reduce(lambda a, b: f'{a}, {b}', map(quote, strings))


COMPATIBLE_LIST: List[str] = [
    'nvidia,p2822-0000+p2888-0001',     # Jetson AGX Xavier Developer Kit
    'nvidia,p3509-0000+p3668-0001',     # Jetson AGX Xavier series
    'nvidia,p3737-0000+p3701-0000',     # Jetson AGX Orin
    'nvidia,p3737-0000+p3701-0000',
    'nvidia,p3737-0000+p3701-0004',
    'nvidia,p3737-0000+p3701-0005',
    'nvidia,tegra234',
    'nvidia,tegra23x',
]


# CSI connector (Jetson AGX Xavier series/Orin)
# Ref: Jetson Linux Developer Guide documentation
# _JETSON_AGX_CSI_CONNECTOR_NAME: Final[str] = 'Jetson AGX Xavier CSI Connector'
#
# However, what /opt/nvidia/jetson-io/Headers/csi-agx.py says is:
_JETSON_AGX_CSI_CONNECTOR_NAME: Final[str] = 'Jetson AGX CSI Connector'


# Create a template for a camera devicetree overlay for Jetson platforms
def jetson_camera_overlay_root(overlay_name: str) -> DeviceTreeNode:
    return (
        DeviceTreeNode('/')
            .header(
                '/dts-v1/;\n'
                '/plugin/;\n')
            .properties([
                f'overlay-name = "{overlay_name}"',
                f'jetson-header-name = "{_JETSON_AGX_CSI_CONNECTOR_NAME}"',
                'compatible = ' + string_list(COMPATIBLE_LIST),
            ])
            .footer('\n/* Generated by TIER IV camera devicetree overlay generation script */\n')
    )
