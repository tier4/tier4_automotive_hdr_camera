#!/usr/bin/env python3

import typing

StringLike = typing.Any


def validate_csi_lanes(csi_lanes: int) -> None:
    if not (1 <= csi_lanes <= 4):
        raise ValueError(f'Invalid number of CSI lanes specified: {csi_lanes}')
