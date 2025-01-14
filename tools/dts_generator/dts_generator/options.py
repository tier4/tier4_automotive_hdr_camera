#!/usr/bin/env python3

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from enum import Enum
from typing import Any, Dict, List, Optional

from . import camera, tegra_nvcsi


@dataclass
class L4T_Version:
    major: int
    minor: Optional[int]
    patch: Optional[int]

    @staticmethod
    def from_string(version: str) -> L4T_Version:
        if version[0].lower() != 'r':
            raise ValueError('L4T version: must start with "R" or "r"')

        version_list = version[1:].split('.', maxsplit=2)
        if '' in version_list:
            raise ValueError('L4T version: empty version number found')

        numbers: List[Optional[str]] = version_list + [None] * (3 - len(version_list))

        major = None if numbers[0] is None else int(numbers[0])
        minor = None if numbers[1] is None else int(numbers[1])
        patch = None if numbers[2] is None else int(numbers[2])

        if major is None:
            raise ValueError('L4T version: at least major version is required')

        return L4T_Version(major, minor, patch)

    def __str__(self) -> str:
        version = f'R{self.major}'
        if self.minor is not None:
            version += f'.{self.minor}'
        if self.patch is not None:
            version += f'.{self.patch}'
        return version


class _OrderedAction(argparse.Action):
    def __call__(self, parser: argparse.ArgumentParser, namespace: argparse.Namespace,
                 values: Any, option_string: Optional[str] = None) -> None:
        if 'ordered_args' not in namespace:
            setattr(namespace, 'ordered_args', [])

        previous = namespace.ordered_args
        previous.append((self.dest, values))
        setattr(namespace, 'ordered_args', previous)


class JetsonSoM(Enum):
    AGX_Xavier = 1
    AGX_Orin = 2


class Board(Enum):
    Xavier_devkit = 1
    Orin_devkit = 2
    ROSCube_58g = 3
    ROSCube_59g = 4
    EAC_5000 = 5
    Anvil = 6

    @property
    def som(self) -> JetsonSoM:
        return _board_som[self]


_board_som: Dict[Board, JetsonSoM] = {
    Board.Xavier_devkit: JetsonSoM.AGX_Xavier,
    Board.Orin_devkit: JetsonSoM.AGX_Orin,
    Board.ROSCube_58g: JetsonSoM.AGX_Xavier,
    Board.ROSCube_59g: JetsonSoM.AGX_Orin,
    Board.EAC_5000: JetsonSoM.AGX_Orin,
    Board.Anvil: JetsonSoM.AGX_Orin,
}


@dataclass
class GeneratorOptions:
    args: argparse.Namespace
    l4t_version: L4T_Version
    _overlay_name: Optional[str]
    _output_path: Optional[str]
    camera_list: List[camera.Type]

    @property
    def number_of_cameras(self) -> int:
        return len(self.camera_list)

    @property
    def overlay_name(self) -> str:
        return self._overlay_name or self.get_default_overlay_name()

    def get_default_overlay_name(self) -> str:
        overlay_name = 'TIERIV GMSL2 Camera Device Tree Overlay:'
        for num, cam in self.args.ordered_args:
            overlay_name += f' {cam[0]}x{num}'

        return overlay_name

    def get_default_overlay_name_compat(self) -> str:
        overlay_name = 'TIERIV '
        if camera.Type.C1 in self.camera_list:
            overlay_name += (camera.Type.C1.image_sensor_name.upper() + ' ')
        if camera.Type.C2 in self.camera_list:
            overlay_name += (camera.Type.C2.image_sensor_name.upper() + ' ')
        if camera.Type.C3 in self.camera_list:
            overlay_name += (camera.Type.C3.image_sensor_name.upper() + ' ')
        overlay_name += 'GMSL2 Camera Device Tree Overlay'

        return overlay_name

    def get_overlay_output_path(self, platform_name: str) -> str:
        return self._output_path or self.get_default_overlay_filename(platform_name)

    def get_default_overlay_filename(self, platform_name: str) -> str:
        dts_filename = 'tier4'
        if camera.Type.C1 in self.camera_list:
            dts_filename += f'-{camera.Type.C1.image_sensor_name}'
        if camera.Type.C2 in self.camera_list:
            dts_filename += f'-{camera.Type.C2.image_sensor_name}'
        if camera.Type.C3 in self.camera_list:
            dts_filename += f'-{camera.Type.C3.image_sensor_name}'

        version = str(self.l4t_version).replace('.', '').lower()
        dts_filename += f'-gmsl-device-tree-overlay-{platform_name.lower()}-{version}.dts'

        return dts_filename

    @classmethod
    def parse_args(cls, description: str) -> GeneratorOptions:
        usage_exmaple = f'Example: {sys.argv[0]} -4 C1 -4 C2\n' \
            'Generate a devicetree overlay for C1 x4 + C2 x4 cameras'
        parser = argparse.ArgumentParser(
            description=description,
            epilog=usage_exmaple,
            formatter_class=argparse.RawTextHelpFormatter)

        parser.add_argument('L4T', type=str, nargs=1,
                            help='target L4T version: e.g. r32.6.1, R35')
        parser.add_argument('-o', '--output', type=str, nargs=1,
                            help='output file path. Write to stdout when "-" is specified')
        parser.add_argument('-n', '--overlay-name', type=str, nargs=1,
                            help='Jetson camera devicetree overlay name')

        camera_arg_help = 'specify the number and type of cameras to use'
        camera_choices = [camera_type.name for camera_type in camera.Type]
        metavar = f'({", ".join(camera_choices)})'

        parser.add_argument('-2', action=_OrderedAction, type=str, nargs=1, metavar=metavar,
                            choices=camera_choices, help=camera_arg_help)
        parser.add_argument('-4', action=_OrderedAction, type=str, nargs=1, metavar=metavar,
                            choices=camera_choices, help=camera_arg_help)
        parser.add_argument('-6', action=_OrderedAction, type=str, nargs=1, metavar=metavar,
                            choices=camera_choices, help=camera_arg_help)
        parser.add_argument('-8', action=_OrderedAction, type=str, nargs=1, metavar=metavar,
                            choices=camera_choices, help=camera_arg_help)

        args = parser.parse_args()

        if not hasattr(args, 'ordered_args'):
            raise ValueError('no cameras were specified')

        camera_list: List[camera.Type] = []
        for num_option, cam in args.ordered_args:
            num, camera_type = int(num_option), camera.Type[cam[0]]
            if len(camera_list) + num > tegra_nvcsi.MAX_CHANNELS:
                raise ValueError('number of cameras specified exceeds the maximum value: %d'
                    % tegra_nvcsi.MAX_CHANNELS)

            camera_list += [camera_type] * num

        return cls(args=args,
                   l4t_version=L4T_Version.from_string(args.L4T[0]),
                   _overlay_name=args.overlay_name and args.overlay_name[0],
                   _output_path=args.output and args.output[0],
                   camera_list=camera_list)
