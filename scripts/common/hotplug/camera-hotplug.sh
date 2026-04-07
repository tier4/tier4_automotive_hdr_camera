#!/bin/bash
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 board L4T-revision overlay-args..."
    echo "  board = orin-devkit, roscube, roscube-orin xavier-devkit"
    echo "  L4T-revision = R32.5.1, R32.5.2, R32.6.1, R35.1 ..."
    exit 1
fi

BOARD=$1
L4T_REVISION=$2
SCRIPT_DIR="$(cd "$(dirname "$(realpath "$0")")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
GENERATOR="$REPO_ROOT/tools/dts_generator/make_overlay_dts_${BOARD}.py"
ADAPT_OVERLAY="$REPO_ROOT/tools/dts_generator/adapt_overlay_for_runtime.py"
DEDUP="$REPO_ROOT/tools/dts_generator/dedup_overlay_dts.py"
OVERLAY_DIR="$SCRIPT_DIR/overlay"

set -eux

mkdir -p "$OVERLAY_DIR"
cd "$OVERLAY_DIR"
rm -f *.dts *.dtbo

# Step 1: Generate DTS source (boot-time-compatible overlay)
"$GENERATOR" "$L4T_REVISION" ${@:3}

# Step 2: Unload camera modules.
# modprobe -r accepts multiple modules; modprobe (load) does NOT — additional
# args are treated as module parameters.  So unload uses multi-module syntax
# but reload (step 6) must use separate calls.
#
# tegra-camera / tegra_camera_platform / nvhost-capture / nvhost-nvcsi cannot
# be unloaded (host1x device bindings hold module refs and the remove() path
# crashes).  They stay resident; the overlay + NVCSI/VI/sensor reload is
# sufficient for re-probing.
sudo modprobe -r tier4-imx728 tier4-imx490 tier4-isx021 2>/dev/null || true
sudo modprobe -r tier4_gw5300 2>/dev/null || true
sudo modprobe -r nvhost-isp5 nvhost-vi5 nvhost-nvcsi-t194 2>/dev/null || true

# Step 2b: Reset GMSL serdes state.
# The MAX9296 deserializer retains link configuration from the previous overlay.
# Without reset, a different sensor type cannot establish the GMSL link.
#
# TODO: This i2cset reset is a workaround. The RESET_ALL should ideally be
#       performed in the kernel driver (e.g. MAX9296 probe or a dedicated
#       reset function) rather than from a shell script. Additionally, whether
#       i2cset is truly necessary (vs serdes module reload alone) has not been
#       verified in a clean environment. See analysis doc "残存課題" for details.
#
# TODO: Bus numbers 10-13 are hardcoded for CTI Anvil. Other boards will
#       have different I2C mux topology.
#
# MAX9296B register 0x0010 (CTRL0):
#   bit 7: RESET_ALL      — full chip reset (equivalent to power cycle)
#   bit 6: RESET_LINK     — reset GMSL PHY, stays in reset until cleared
#   bit 5: RESET_ONESHOT  — reset GMSL PHY + data pipelines, auto-clears
#
# Send RESET_ALL BEFORE unloading serdes modules — the I2C mux routing is
# only reliable while the kernel drivers are still managing the bus.
# After reset, the link takes up to 100ms to re-establish (datasheet).
for bus in 10 11 12 13; do
    sudo i2cset -f -y "$bus" 0x48 0x00 0x10 0x80 i 2>/dev/null || true
done
sleep 3
sudo modprobe -r tier4_max9296 tier4_max9295 2>/dev/null || true

sudo rmdir /sys/kernel/config/device-tree/overlays/camera/ 2>/dev/null || true

# Step 3: Adapt overlay for kernel runtime compatibility.
# The generator produces a self-contained boot-time overlay with embedded
# base DTSI.  The kernel's overlay infrastructure is stricter than the
# bootloader: it pre-resolves all target-path values and rejects duplicate
# property updates.  adapt_overlay_for_runtime.py transforms the overlay by
# resolving target-paths against the live device tree and merging fragments
# that target the same node.
sudo python3 "$ADAPT_OVERLAY" --in-place -v *.dts

# Step 4: Dedup labels that conflict with the live device tree's __symbols__
# to avoid phandle conflicts in overlay.c:add_changeset_node().
sudo python3 "$DEDUP" --in-place -v *.dts

# Step 5: Compile and apply the overlay
sudo dtc -O dtb -o target.dtbo -@ $(ls *.dts)
sudo mkdir -p /sys/kernel/config/device-tree/overlays/camera/
sudo cp target.dtbo /sys/kernel/config/device-tree/overlays/camera/dtbo
echo 1 | sudo tee /sys/kernel/config/device-tree/overlays/camera/status

# Step 6: Reload camera modules.
# modprobe (load) only accepts ONE module per invocation; additional args are
# treated as module parameters.  Each module must be loaded separately.
# Reload serdes first (must be present before sensor drivers probe).
sudo modprobe tier4_max9295
sudo modprobe tier4_max9296
sleep 2
sudo modprobe nvhost-nvcsi-t194
sudo modprobe nvhost-vi5
sudo modprobe nvhost-isp5
sudo modprobe tier4-imx728 2>/dev/null || true
sudo modprobe tier4-imx490 2>/dev/null || true
sudo modprobe tier4-isx021 2>/dev/null || true
