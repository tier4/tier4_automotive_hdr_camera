#!/usr/bin/env python3
"""Adapt DTS overlay for kernel runtime application.

The Linux kernel's overlay infrastructure resolves ALL target-path values
before applying any fragments (unlike bootloaders which apply sequentially).
It also rejects overlays where multiple fragments modify the same property.

This tool transforms a boot-time-compatible DTS overlay for runtime use:
  1. Resolves target-path values against the live device tree
  2. Wraps fragments whose target-path doesn't exist in the live tree
  3. Merges fragments targeting the same path (last-writer-wins)

NOTE: This script must run on the target device because it reads the live
device tree from /proc/device-tree/ to resolve target-path values.

Usage:
  adapt_overlay_for_runtime.py [--in-place] [-v] [--dt-dir DIR] file.dts [...]
"""

import argparse
import os
import re
import sys
from collections import OrderedDict
from typing import Dict, List, Optional, Sequence, Tuple

DEFAULT_DT_DIR = '/proc/device-tree'


# ---------------------------------------------------------------------------
# Low-level DTS text helpers
# ---------------------------------------------------------------------------

def _skip_comment(text: str, pos: int) -> int:
    """Skip a comment starting at *pos*.  Returns position after it."""
    if text[pos:pos + 2] == '//':
        end = text.find('\n', pos)
        return (end + 1) if end != -1 else len(text)
    if text[pos:pos + 2] == '/*':
        end = text.find('*/', pos + 2)
        return (end + 2) if end != -1 else len(text)
    return pos


def _find_matching_brace(text: str, start: int) -> int:
    """Return index of the ``}`` that matches the ``{`` at *start*."""
    assert text[start] == '{', f'Expected {{ at {start}, got {text[start]!r}'
    depth = 1
    pos = start + 1
    while pos < len(text) and depth > 0:
        ch = text[pos]
        if ch == '/' and pos + 1 < len(text) and text[pos + 1] in '/*':
            pos = _skip_comment(text, pos)
            continue
        if ch == '"':
            pos += 1
            while pos < len(text) and text[pos] != '"':
                if text[pos] == '\\':
                    pos += 1
                pos += 1
            pos += 1
            continue
        if ch == '{':
            depth += 1
        elif ch == '}':
            depth -= 1
        pos += 1
    return pos - 1


# ---------------------------------------------------------------------------
# Overlay node tree (for merging)
# ---------------------------------------------------------------------------

class DtsNode:
    """Minimal representation of a DTS node for deep-merging overlays."""

    __slots__ = ('label', 'properties', 'children')

    def __init__(self) -> None:
        self.label: str = ''
        self.properties: List[Tuple[str, str]] = []   # (key, full_text)
        self.children: 'OrderedDict[str, DtsNode]' = OrderedDict()


def _prop_key(full_text: str) -> str:
    """Extract the property name from a full property string."""
    eq = full_text.find('=')
    if eq != -1:
        return full_text[:eq].strip()
    return full_text.split()[0] if full_text.strip() else ''


def _parse_node_body(text: str) -> DtsNode:
    """Parse the text *between* braces of a DTS node."""
    node = DtsNode()
    pos = 0
    length = len(text)

    while pos < length:
        # skip whitespace and semicolons
        while pos < length and text[pos] in ' \t\n\r;':
            pos += 1
        if pos >= length:
            break

        # skip comments
        if text[pos] == '/' and pos + 1 < length and text[pos + 1] in '/*':
            pos = _skip_comment(text, pos)
            continue

        # Try to match a child node: [label :] name { body };
        m = re.match(r'((\w+)\s*:\s*)?([\w@,.+#/-]+)\s*\{', text[pos:])
        if m:
            label = m.group(2) or ''
            name = m.group(3)
            brace_start = pos + m.end() - 1
            brace_end = _find_matching_brace(text, brace_start)
            child_body = text[brace_start + 1:brace_end]

            child = _parse_node_body(child_body)
            child.label = label

            if name in node.children:
                _merge_nodes(node.children[name], child)
            else:
                node.children[name] = child

            pos = brace_end + 1
            continue

        # Match a property (everything up to the next unquoted `;`)
        prop_start = pos
        while pos < length:
            ch = text[pos]
            if ch == ';':
                break
            if ch == '"':
                pos += 1
                while pos < length and text[pos] != '"':
                    if text[pos] == '\\':
                        pos += 1
                    pos += 1
            if ch == '{':
                # Shouldn't happen if well-formed, but bail out
                break
            pos += 1

        if pos > prop_start and text[pos:pos + 1] == ';':
            prop_text = text[prop_start:pos].strip()
            if prop_text:
                key = _prop_key(prop_text)
                node.properties.append((key, prop_text))
            pos += 1
            continue

        pos += 1

    return node


def _merge_nodes(base: DtsNode, overlay: DtsNode) -> None:
    """Deep-merge *overlay* into *base*.  Overlay values win."""
    if overlay.label:
        base.label = overlay.label

    # Build map of existing base properties by key
    existing: Dict[str, int] = {}
    for i, (key, _) in enumerate(base.properties):
        if key not in existing:
            existing[key] = i

    # Update existing properties in-place, append new ones
    for key, text in overlay.properties:
        if key in existing:
            idx = existing[key]
            base.properties[idx] = (key, text)
        else:
            base.properties.append((key, text))
            existing[key] = len(base.properties) - 1

    # Merge children recursively
    for name, child in overlay.children.items():
        if name in base.children:
            _merge_nodes(base.children[name], child)
        else:
            base.children[name] = child


def _serialize_node(node: DtsNode, indent: str = '') -> str:
    """Serialize a DtsNode back to DTS text."""
    parts: List[str] = []
    for _, prop_text in node.properties:
        parts.append(f'{indent}\t{prop_text};')
    for name, child in node.children.items():
        label_prefix = f'{child.label}: ' if child.label else ''
        parts.append(f'{indent}\t{label_prefix}{name} {{')
        parts.append(_serialize_node(child, indent + '\t'))
        parts.append(f'{indent}\t}};')
    return '\n'.join(parts)


# ---------------------------------------------------------------------------
# Fragment extraction and reassembly
# ---------------------------------------------------------------------------

class Fragment:
    """A parsed DTS overlay fragment."""
    __slots__ = ('name', 'target_path', 'target_label', 'overlay_body')

    def __init__(self, name: str, target_path: Optional[str],
                 target_label: Optional[str], overlay_body: str) -> None:
        self.name = name
        self.target_path = target_path
        self.target_label = target_label
        self.overlay_body = overlay_body


_FRAG_RE = re.compile(r'([\w-]+@\d+)\s*\{')
_TARGET_PATH_RE = re.compile(r'target-path\s*=\s*"([^"]*)"')
_TARGET_LABEL_RE = re.compile(r'target\s*=\s*<&(\w+)>')


def _extract_fragments(dts_text: str) -> Tuple[str, List[Fragment]]:
    """Extract all fragments from *dts_text*.

    Returns ``(preamble, fragments)`` where *preamble* is the text before
    the first ``/ {`` block (``/dts-v1/;``, ``/plugin/;``, comments, etc.).
    """
    fragments: List[Fragment] = []

    # Find all root blocks: / { ... };
    root_re = re.compile(r'(?:^|\n)(\s*/\s*)\{')
    preamble_end = 0
    preamble = ''

    for match in root_re.finditer(dts_text):
        if preamble_end == 0:
            preamble = dts_text[:match.start()]
        brace_pos = match.start() + match.group(0).index('{')
        if dts_text[match.start()] == '\n':
            brace_pos = match.start() + match.group(0).index('{')
        brace_end = _find_matching_brace(dts_text, brace_pos)
        root_body = dts_text[brace_pos + 1:brace_end]
        preamble_end = brace_end + 1

        # Parse fragments at depth 0 inside the root block
        pos = 0
        while pos < len(root_body):
            while pos < len(root_body) and root_body[pos] in ' \t\n\r;':
                pos += 1
            if pos >= len(root_body):
                break
            if root_body[pos] == '/' and pos + 1 < len(root_body) and root_body[pos + 1] in '/*':
                pos = _skip_comment(root_body, pos)
                continue

            m = _FRAG_RE.match(root_body, pos)
            if not m:
                pos += 1
                continue

            frag_name = m.group(1)
            fb_start = root_body.index('{', m.start())
            fb_end = _find_matching_brace(root_body, fb_start)
            frag_body = root_body[fb_start + 1:fb_end]

            target_path: Optional[str] = None
            target_label: Optional[str] = None
            tp = _TARGET_PATH_RE.search(frag_body)
            if tp:
                target_path = tp.group(1)
            else:
                tl = _TARGET_LABEL_RE.search(frag_body)
                if tl:
                    target_label = tl.group(1)

            ov = re.search(r'__overlay__\s*\{', frag_body)
            if ov:
                ov_brace = frag_body.index('{', ov.start())
                ov_end = _find_matching_brace(frag_body, ov_brace)
                overlay_body = frag_body[ov_brace + 1:ov_end]
                fragments.append(Fragment(frag_name, target_path,
                                          target_label, overlay_body))

            pos = fb_end + 1

    return preamble, fragments


# ---------------------------------------------------------------------------
# Path resolution
# ---------------------------------------------------------------------------

def _resolve_path(target_path: str, dt_dir: str) -> Tuple[str, List[str]]:
    """Find the longest existing prefix of *target_path* in the live DT.

    Returns ``(existing_prefix, remaining_components)``.
    """
    path = target_path.rstrip('/')
    if path == '' or path == '/':
        return '/', []
    components = path.split('/')[1:]  # drop leading ''

    for i in range(len(components), 0, -1):
        prefix = '/' + '/'.join(components[:i])
        if os.path.isdir(os.path.join(dt_dir, prefix.lstrip('/'))):
            return prefix, list(components[i:])

    return '/', list(components)


def _wrap_body(overlay_body: str, remaining: List[str]) -> str:
    """Wrap *overlay_body* in nested node blocks for *remaining* path parts."""
    body = overlay_body
    for component in reversed(remaining):
        body = f'\n\t\t\t{component} {{{body}\n\t\t\t}};'
    return body


# ---------------------------------------------------------------------------
# Main adaptation logic
# ---------------------------------------------------------------------------

def adapt_overlay(dts_text: str, dt_dir: str, verbose: bool = False) -> str:
    """Adapt *dts_text* for kernel runtime overlay application."""
    preamble, fragments = _extract_fragments(dts_text)

    if not os.path.isdir(dt_dir):
        if verbose:
            print(f'  [adapt] {dt_dir} not found, skipping',
                  file=sys.stderr)
        return dts_text

    # Phase 1: resolve target-paths and wrap overlay bodies
    resolved: List[Tuple[str, Fragment]] = []  # (resolved_path, fragment)
    for frag in fragments:
        if frag.target_label is not None:
            # target = <&label> — pass through unchanged
            resolved.append(('__label__:' + frag.target_label, frag))
            continue

        if frag.target_path is None:
            continue

        existing, remaining = _resolve_path(frag.target_path, dt_dir)
        if remaining:
            if verbose:
                orig = frag.target_path
                print(f'  [adapt] {orig} -> {existing} + /{"/".join(remaining)}',
                      file=sys.stderr)
            frag.overlay_body = _wrap_body(frag.overlay_body, remaining)
            frag.target_path = existing
        resolved.append((frag.target_path, frag))

    # Phase 2: group fragments by resolved target-path
    groups: OrderedDict[str, List[Fragment]] = OrderedDict()
    for key, frag in resolved:
        groups.setdefault(key, []).append(frag)

    # Phase 3: merge groups
    merged_fragments: List[Tuple[str, Fragment]] = []
    for key, group in groups.items():
        if len(group) == 1:
            merged_fragments.append((key, group[0]))
            continue

        if verbose:
            names = ', '.join(f.name for f in group)
            print(f'  [adapt] merging {len(group)} fragments '
                  f'targeting {key}: {names}', file=sys.stderr)

        # Deep merge all overlay bodies
        base_node = _parse_node_body(group[0].overlay_body)
        for frag in group[1:]:
            overlay_node = _parse_node_body(frag.overlay_body)
            _merge_nodes(base_node, overlay_node)

        merged_body = _serialize_node(base_node, '\t\t')
        result_frag = Fragment(
            name=group[0].name,
            target_path=group[0].target_path if not key.startswith('__label__:') else None,
            target_label=group[0].target_label if key.startswith('__label__:') else None,
            overlay_body='\n' + merged_body + '\n\t\t' if merged_body else '',
        )
        merged_fragments.append((key, result_frag))

    # Phase 4: reassemble DTS
    parts = [preamble.rstrip('\n'), '\n/ {\n']
    for i, (key, frag) in enumerate(merged_fragments):
        if key.startswith('__label__:'):
            label = key[len('__label__:'):]
            parts.append(f'\tfragment@{i} {{\n')
            parts.append(f'\t\ttarget = <&{label}>;\n')
        else:
            parts.append(f'\tfragment@{i} {{\n')
            parts.append(f'\t\ttarget-path = "{frag.target_path}";\n')
        parts.append(f'\t\t__overlay__ {{{frag.overlay_body}}};\n')
        parts.append(f'\t}};\n')
    parts.append('};\n')

    return ''.join(parts)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description='Adapt DTS overlay for kernel runtime application.')
    ap.add_argument('files', nargs='+', metavar='FILE',
                    help='DTS files to process')
    ap.add_argument('--in-place', action='store_true',
                    help='Overwrite input files')
    ap.add_argument('-v', '--verbose', action='store_true',
                    help='Print diagnostic messages to stderr')
    ap.add_argument('--dt-dir', default=DEFAULT_DT_DIR,
                    help='Path to the live device tree '
                         f'(default: {DEFAULT_DT_DIR})')
    args = ap.parse_args()

    for path in args.files:
        if args.verbose:
            print(f'adapt: {path}', file=sys.stderr)
        with open(path, 'r', encoding='utf-8') as f:
            original = f.read()

        result = adapt_overlay(original, args.dt_dir, verbose=args.verbose)

        if args.in_place:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(result)
        else:
            sys.stdout.write(result)


if __name__ == '__main__':
    main()
