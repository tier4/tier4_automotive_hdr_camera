#!/usr/bin/env python3
"""Strip duplicate label definitions from DTS overlay files.

Reads the base device-tree symbol names from /proc/device-tree/__symbols__/
and removes label definitions (``label: node {``) from the input DTS files
where the label already exists in the base tree.  This prevents phandle
conflicts and __symbols__ duplicates when applying overlays to boards whose
base DTB pre-defines those labels (e.g. CTI Anvil).

All ``<&label>`` phandle references are preserved.  After label stripping,
``dtc -@`` can no longer resolve them locally and instead emits __fixups__
entries, which the kernel resolves against the base tree at apply time.

NOTE: This script must run on the target device because it reads label names
from /proc/device-tree/__symbols__/ to detect conflicts with the live tree.

Usage:
    dedup_overlay_dts.py [--base-symbols-dir DIR] [--in-place] [-v] [input ...]

If --base-symbols-dir is missing (e.g. cross-compiling on a host), the script
warns to stderr and passes through unchanged.
"""

import argparse
import os
import re
import sys

# Matches DTS label definitions:  ``  vi_in0: endpoint {``
#   group(1) = leading whitespace
#   group(2) = label name
#   group(3) = rest of the line (node-name and anything after)
LABEL_DEF_RE = re.compile(r'^(\s*)(\w+):\s+(\S+)', re.MULTILINE)

# Matches phandle references:  ``<&some_label>``
LABEL_REF_RE = re.compile(r'<&(\w+)>')


def read_base_symbols(symbols_dir):
    """Return the set of symbol names present in the base device tree."""
    try:
        return set(os.listdir(symbols_dir))
    except FileNotFoundError:
        return None
    except PermissionError:
        return None


def strip_labels(dts_text, base_symbols, verbose=False):
    """Remove label definitions for labels that exist in *base_symbols*.

    Returns (modified_text, set_of_stripped_labels).
    """
    stripped = set()

    def replacer(m):
        indent = m.group(1)
        label = m.group(2)
        rest = m.group(3)
        if label in base_symbols:
            stripped.add(label)
            return indent + rest
        return m.group(0)

    result = LABEL_DEF_RE.sub(replacer, dts_text)

    if verbose and stripped:
        names = ', '.join(sorted(stripped))
        print(f'stripped {len(stripped)} labels: {names}', file=sys.stderr)

    return result, stripped


def find_remaining_local_labels(dts_text):
    """Return the set of labels still defined locally in *dts_text*."""
    return {m.group(2) for m in LABEL_DEF_RE.finditer(dts_text)}


def validate_references(dts_text, base_symbols, stripped_labels):
    """Check that no ``<&label>`` reference is orphaned.

    A reference is orphaned if its label was stripped locally AND does not
    exist in the base tree symbols (so it can't be resolved as a __fixups__
    entry either).

    Returns a list of orphaned label names (empty if all OK).
    """
    local_labels = find_remaining_local_labels(dts_text)
    referenced = {m.group(1) for m in LABEL_REF_RE.finditer(dts_text)}

    orphaned = []
    for ref in sorted(referenced):
        if ref in local_labels:
            continue  # still defined locally
        if ref in base_symbols:
            continue  # will become a __fixups__ entry
        orphaned.append(ref)

    return orphaned


def process_file(path, base_symbols, in_place=False, verbose=False):
    """Process a single DTS file.  Returns True on success."""
    with open(path, 'r') as f:
        original = f.read()

    modified, stripped = strip_labels(original, base_symbols, verbose=verbose)

    orphaned = validate_references(modified, base_symbols, stripped)
    if orphaned:
        names = ', '.join(orphaned)
        print(f'ERROR: {path}: orphaned references after dedup: {names}',
              file=sys.stderr)
        return False

    if in_place:
        with open(path, 'w') as f:
            f.write(modified)
    else:
        sys.stdout.write(modified)

    return True


def main():
    parser = argparse.ArgumentParser(
        description='Strip duplicate label definitions from DTS overlay files.')
    parser.add_argument('input_files', nargs='*', metavar='FILE',
                        help='DTS files to process (default: stdin)')
    parser.add_argument('--base-symbols-dir', default='/proc/device-tree/__symbols__/',
                        help='Directory containing base tree symbol names '
                             '(default: /proc/device-tree/__symbols__/)')
    parser.add_argument('-i', '--in-place', action='store_true',
                        help='Modify files in place (default: write to stdout)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print stripped labels to stderr')
    args = parser.parse_args()

    base_symbols = read_base_symbols(args.base_symbols_dir)
    if base_symbols is None:
        print(f'WARNING: {args.base_symbols_dir} not found or not readable; '
              f'passing through unchanged', file=sys.stderr)
        # Pass through: just cat input to output
        for path in args.input_files:
            with open(path, 'r') as f:
                sys.stdout.write(f.read())
        return 0

    if not args.input_files:
        # Read from stdin
        original = sys.stdin.read()
        modified, stripped = strip_labels(original, base_symbols,
                                         verbose=args.verbose)
        orphaned = validate_references(modified, base_symbols, stripped)
        if orphaned:
            names = ', '.join(orphaned)
            print(f'ERROR: <stdin>: orphaned references after dedup: {names}',
                  file=sys.stderr)
            return 1
        sys.stdout.write(modified)
        return 0

    ok = True
    for path in args.input_files:
        if not process_file(path, base_symbols, in_place=args.in_place,
                            verbose=args.verbose):
            ok = False

    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
