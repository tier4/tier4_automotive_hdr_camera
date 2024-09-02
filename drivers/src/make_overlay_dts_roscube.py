#!/usr/bin/env python3

import sys
 
MAX_NUM_CAMERAS = 8
EINVALID_CAMERAS = 1

str_overlay_header_r3541 = """
/dts-v1/;
/plugin/;
 
/ {
    overlay-name = \"TIERIV ISX021 IMX490 GMSL2 Camera Device Tree Overlay\";
    compatible = \"nvidia,p2822-0000+p2888-0001\";
    jetson-header-name = \"Jetson AGX Xavier CSI Connector\";
"""

str_overlay_header_r325 = """
/dts-v1/;
/plugin/;
 
/ {
    overlay-name = \"TIERIV ISX021 IMX490 GMSL2 Camera Device Tree Overlay\";
    compatible = \"nvidia,p2822-0000+p2888-0001\";
    jetson-header-name = \"Jetson AGX Xavier CSI Connector\";

"""

dict_overlay_header = {"325x": str_overlay_header_r325, "3541": str_overlay_header_r3541}

# ================= VI R32.5 =================

str_fragment_vi_0_r325 = """
    fragment@1 {
      target = <&vi_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@2 {
      target = <&vi_in0>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&csi_out0>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_1_r325 = """
    fragment@3 {
      target = <&vi_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@4 {
      target = <&vi_in1>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&csi_out1>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_2_r325 = """
    fragment@5 {
      target = <&vi_port2>;
      __overlay__ {
        reg = <2>;
        status = \"okay\";
      };
    };

    fragment@6 {
      target = <&vi_in2>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&csi_out2>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_3_r325 = """
    fragment@7 {
      target = <&vi_port3>;
      __overlay__ {
        reg = <3>;
        status = \"okay\";
      };
    };

    fragment@8 {
      target = <&vi_in3>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&csi_out3>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_4_r325 = """
    fragment@9 {
      target = <&vi_port4>;
      __overlay__ {
        reg = <4>;
        status = \"okay\";
      };
    };

    fragment@10 {
      target = <&vi_in4>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&csi_out4>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_5_r325 = """
    fragment@11 {
      target = <&vi_port5>;
      __overlay__ {
        reg = <5>;
        status = \"okay\";
      };
    };

    fragment@12 {
      target = <&vi_in5>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&csi_out5>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_6_r325 = """
    fragment@13 {
      target = <&vi_port6>;
      __overlay__ {
        reg = <6>;
        status = \"okay\";
      };
    };

    fragment@14 {
      target = <&vi_in6>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <5>;
        bus-width = <2>;
        remote-endpoint = <&csi_out6>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_7_r325 = """
    fragment@15 {
      target = <&vi_port7>;
      __overlay__ {
        reg = <7>;
        status = \"okay\";
      };
    };

    fragment@16 {
      target = <&vi_in7>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <5>;
        bus-width = <2>;
        remote-endpoint = <&csi_out7>;
      };
    };
"""

# -----------------------------------------------

str_fragment_vi_others_r325 = """
  fragment@17 {
    target-path = "/host1x/vi@15c10000";
    __overlay__ {
      status = "okay";
      num-channels = <8>;
    };
  };
"""

# ================== VI R35.4.1 ===================

str_fragment_vi_0_r3541 = """

//  ------------ for VI ---------

  fragment@1{
    target-path = \"/tegra-capture-vi\";
    __overlay__ {
      status = \"okay\";
      num-channels = <0x08>;
    };
  };

//diff view
  fragment@2{
    target-path = \"/tegra-capture-vi/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@3{
    target-path = \"/tegra-capture-vi/ports/port@0/endpoint\";
    __overlay__ {
      status = \"okay\";
      port-index = <0>;
      vc-id = <0>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_1_r3541 = """
  fragment@4{
    target-path = \"/tegra-capture-vi/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@5{
    target-path = \"/tegra-capture-vi/ports/port@1/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <1>;
      port-index = <0>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_2_r3541 = """
  fragment@6{
    target-path = \"/tegra-capture-vi/ports/port@2\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@7{
    target-path = \"/tegra-capture-vi/ports/port@2/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <0>;
      port-index = <2>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_3_r3541 = """
  fragment@8 {
    target-path = \"/tegra-capture-vi/ports/port@3\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@9 {
    target-path = \"/tegra-capture-vi/ports/port@3/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <1>;
      port-index = <2>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_4_r3541 = """
  fragment@10 {
    target-path = \"/tegra-capture-vi/ports/port@4\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@11 {
    target-path = \"/tegra-capture-vi/ports/port@4/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <0>;
      port-index = <4>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_5_r3541 = """
  fragment@12 {
    target-path = \"/tegra-capture-vi/ports/port@5\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@13 {
    target-path = \"/tegra-capture-vi/ports/port@5/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <1>;
      port-index = <4>;
      bus-width = <2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_vi_6_r3541 = """
  fragment@14 {
    target-path = \"/tegra-capture-vi/ports/port@6\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@15 {
    target-path = \"/tegra-capture-vi/ports/port@6/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <0>;
      port-index = <5>;
      bus-width = <2>;
    };
  };
"""

str_fragment_vi_7_r3541 = """
  fragment@16 {
    target-path = \"/tegra-capture-vi/ports/port@7\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@17 {
    target-path = \"/tegra-capture-vi/ports/port@7/endpoint\";
    __overlay__ {
      status = \"okay\";
      vc-id = <1>;
      port-index = <5>;
      bus-width = <2>;
    };
  };
"""

str_fragment_vi_others_r3541 = """
  fragment@18 {
    target-path = \"/host1x@13e00000/vi@15c10000\";
    __overlay__ {
      num-channels = <8>;
      status = \"okay\";
    };
  };
"""

dict_fragment_vi_0 = {"325x": str_fragment_vi_0_r325, "3541": str_fragment_vi_0_r3541}
dict_fragment_vi_1 = {"325x": str_fragment_vi_1_r325, "3541": str_fragment_vi_1_r3541}
dict_fragment_vi_2 = {"325x": str_fragment_vi_2_r325, "3541": str_fragment_vi_2_r3541}
dict_fragment_vi_3 = {"325x": str_fragment_vi_3_r325, "3541": str_fragment_vi_3_r3541}
dict_fragment_vi_4 = {"325x": str_fragment_vi_4_r325, "3541": str_fragment_vi_4_r3541}
dict_fragment_vi_5 = {"325x": str_fragment_vi_5_r325, "3541": str_fragment_vi_5_r3541}
dict_fragment_vi_6 = {"325x": str_fragment_vi_6_r325, "3541": str_fragment_vi_6_r3541}
dict_fragment_vi_7 = {"325x": str_fragment_vi_7_r325, "3541": str_fragment_vi_7_r3541}

dict_fragment_vi_others = {
    "325x": str_fragment_vi_others_r325,
    "3541": str_fragment_vi_others_r3541,
}

# ================= NVCSI R32.5 =================

str_fragment_nvcsi_ch0_r325 = """

// -----  NVCSI  ------

    /* channel 0 */
    fragment@20 {
      target = <&csi_chan0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@21 {
      target = <&csi_chan0_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@22 {
      target = <&csi_chan0_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@23 {
      target = <&csi_in0>;
      __overlay__ {
        status = \"okay\";
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out0>;
      };
    };

    fragment@24 {
      target = <&csi_out0>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in0>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch1_r325 = """
    /* channel 1 */
    fragment@25 {
      target = <&csi_chan1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@26 {
      target = <&csi_chan1_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@27 {
      target = <&csi_chan1_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@28 {
      target = <&csi_in1>;
      __overlay__ {
        status = \"okay\";
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out1>;
      };
    };

    fragment@29 {
      target = <&csi_out1>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in1>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch2_r325 = """
    /* channel 2 */
    fragment@30 {
      target = <&csi_chan2>;
      __overlay__ {
        reg = <2>;
        status = \"okay\";
      };
    };

    fragment@31 {
      target = <&csi_chan2_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@32 {
      target = <&csi_chan2_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@33 {
      target = <&csi_in2>;
      __overlay__ {
        status = \"okay\";
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out2>;
      };
    };

    fragment@34 {
      target = <&csi_out2>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in2>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch3_r325 = """
    /* channel 3 */
    fragment@35 {
      target = <&csi_chan3>;
      __overlay__ {
        reg = <3>;
        status = \"okay\";
      };
    };

    fragment@36 {
      target = <&csi_chan3_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@37 {
      target = <&csi_chan3_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@38 {
      target = <&csi_in3>;
      __overlay__ {
        status = \"okay\";
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out3>;
      };
    };

    fragment@39 {
      target = <&csi_out3>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in3>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch4_r325 = """
    /* channel 4 */
    fragment@40 {
      target = <&csi_chan4>;
      __overlay__ {
        reg = <4>;
        status = \"okay\";
      };
    };

    fragment@41 {
      target = <&csi_chan4_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@42 {
      target = <&csi_chan4_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@43 {
      target = <&csi_in4>;
      __overlay__ {
        status = \"okay\";
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out4>;
      };
    };

    fragment@44 {
      target = <&csi_out4>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in4>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch5_r325 = """
    /* channel 5 */
    fragment@45 {
      target = <&csi_chan5>;
      __overlay__ {
        reg = <5>;
        status = \"okay\";
      };
    };

    fragment@46 {
      target = <&csi_chan5_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@47 {
      target = <&csi_chan5_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@48 {
      target = <&csi_in5>;
      __overlay__ {
        status = \"okay\";
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out5>;
      };
    };

    fragment@49 {
      target = <&csi_out5>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in5>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch6_r325 = """
    /* channel 6 */
    fragment@50 {
      target = <&csi_chan6>;
      __overlay__ {
        reg = <6>;
        status = \"okay\";
      };
    };

    fragment@51 {
      target = <&csi_chan6_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@52 {
      target = <&csi_chan6_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@53 {
      target = <&csi_in6>;
      __overlay__ {
        status = \"okay\";
        port-index = <6>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out6>;
      };
    };

    fragment@54 {
      target = <&csi_out6>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in6>;
      };
    };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch7_r325 = """
    /* channel 7 */
    fragment@55 {
      target = <&csi_chan7>;
      __overlay__ {
        reg = <7>;
        status = \"okay\";
      };
    };

    fragment@56 {
      target = <&csi_chan7_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@57 {
      target = <&csi_chan7_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@58 {
      target = <&csi_in7>;
      __overlay__ {
        status = \"okay\";
        port-index = <6>;
        bus-width = <2>;
        remote-endpoint = <&isx021_out7>;
      };
    };

    fragment@59 {
      target = <&csi_out7>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in7>;
      };
    };
"""

str_fragment_nvcsi_others_r325 = """
  fragment@60 {
    target = <&nvcsi>;
    __overlay__ {
      status = "okay";
      num-channels = <8>;
    };
  };
"""


# ================= NVCSI R35.4.1 =================


str_fragment_nvcsi_ch0_r3541 = """
// -----   NVCSI  ------

// channel@0

  fragment@20 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@21 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@0/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };
  fragment@22 {
    // target = csi_in0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@0/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      bus-width = <0x02>;
      remote-endpoint = <&isx021_out0>;
    };
  };

  fragment@23 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@0/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@24 {
    // target = csi_out0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@0/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in0>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch1_r3541 = """
// channel@1

  fragment@25 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@26 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@1/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@27 {
    // target = csi_in1
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@1/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <0x00>;
      bus-width = <0x02>;
      remote-endpoint = <&isx021_out1>;
    };
  };

  fragment@28 {
    // target = csi_out1
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@1/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in1>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch2_r3541 = """
// channel@2

  fragment@29 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@2\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@30 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@2/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@31 {
    // target = csi_in0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@2/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <2>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out2>;
    };
  };

  fragment@32 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@2/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@33 {
    // target = csi_out0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@2/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in2>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch3_r3541 = """
// channel@3

  fragment@34 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@3\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@35 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@3/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@36 {
    // target = csi_in0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@3/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <2>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out3>;
    };
  };

  fragment@37 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@3/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@38 {
    // target = csi_out0
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@3/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in3>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch4_r3541 = """
// channel@4

  fragment@39 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@4\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@40 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@4/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@41 {
    // target = csi_in4
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@4/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <4>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out4>;
    };
  };

  fragment@42 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@4/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@43 {
    // target = csi_out4
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@4/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in4>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch5_r3541 = """
// channel@5

  fragment@44 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@5\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@45 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@5/ports/port@0\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@46 {
    // target = csi_in5
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@5/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <4>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out5>;
    };
  };

  fragment@47 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@5/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@48 {
    // target = csi_out5
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@5/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in5>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch6_r3541 = """
// channel@6

  fragment@49 {
    // target = csi_in6
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@6/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <6>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out6>;
    };
  };

  fragment@50 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@6/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@51 {
    // target = csi_out6
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@6/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in6>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_ch7_r3541 = """
// channel@7

  fragment@52 {
    // target = csi_in7
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@7/ports/port@0/endpoint@0\";
    __overlay__ {
      status = \"okay\";
      port-index = <6>;
      bus-width = <2>;
      remote-endpoint = <&isx021_out7>;
    };
  };

  fragment@53 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@7/ports/port@1\";
    __overlay__ {
      status = \"okay\";
    };
  };

  fragment@54 {
    // target = csi_out7
    target-path = \"/host1x@13e00000/nvcsi@15a00000/channel@7/ports/port@1/endpoint@1\";
    __overlay__ {
      status = \"okay\";
      remote-endpoint = <&vi_in7>;
    };
  };
"""

# -----------------------------------------------

str_fragment_nvcsi_others_r3541 = """
  fragment@55 {
    target-path = \"/host1x@13e00000/nvcsi@15a00000\";
    __overlay__ {
      status = \"okay\";
      num-channels = <8>;
    };
  };

  fragment@56 {
    target-path = \"/host1x@13e00000/slvs-ec@15ac0000\";
    __overlay__ {
      status = \"okay\";
    };
  };

"""

dict_fragment_nvcsi_ch0 = {
    "325x": str_fragment_nvcsi_ch0_r325,
    "3541": str_fragment_nvcsi_ch0_r3541,
}
dict_fragment_nvcsi_ch1 = {
    "325x": str_fragment_nvcsi_ch1_r325,
    "3541": str_fragment_nvcsi_ch1_r3541,
}
dict_fragment_nvcsi_ch2 = {
    "325x": str_fragment_nvcsi_ch2_r325,
    "3541": str_fragment_nvcsi_ch2_r3541,
}
dict_fragment_nvcsi_ch3 = {
    "325x": str_fragment_nvcsi_ch3_r325,
    "3541": str_fragment_nvcsi_ch3_r3541,
}
dict_fragment_nvcsi_ch4 = {
    "325x": str_fragment_nvcsi_ch4_r325,
    "3541": str_fragment_nvcsi_ch4_r3541,
}
dict_fragment_nvcsi_ch5 = {
    "325x": str_fragment_nvcsi_ch5_r325,
    "3541": str_fragment_nvcsi_ch5_r3541,
}
dict_fragment_nvcsi_ch6 = {
    "325x": str_fragment_nvcsi_ch6_r325,
    "3541": str_fragment_nvcsi_ch6_r3541,
}
dict_fragment_nvcsi_ch7 = {
    "325x": str_fragment_nvcsi_ch7_r325,
    "3541": str_fragment_nvcsi_ch7_r3541,
}

dict_fragment_nvcsi_others = {
    "325x": str_fragment_nvcsi_others_r325,
    "3541": str_fragment_nvcsi_others_r3541,
}

# ============ CAMERA MODULES  R32.5 ============


str_fragment_camera_module_r325 = """

// ----- Camera modules -----

 /* camera 0 */
 fragment@70 {
   target-path = \"/tegra-camera-platform\";
   __overlay__ {
     status = \"okay\";
     num_csi_lanes = <0x02>;
     max_lane_speed = <4000000>;
   };
 };
"""

str_fragment_isx021_camera_module0_r325 = """
 fragment@71 {
   target-path = \"/tegra-camera-platform/modules/module0\";
   __overlay__ {
     badge = \"isx021_rear\";
     position = \"rear\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@72 {
   target-path = \"/tegra-camera-platform/modules/module0/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 30-001b\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_a@1b\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module1_r325 = """
 fragment@73 {
   target-path = \"/tegra-camera-platform/modules/module1\";
   __overlay__ {
     badge = \"isx021_front\";
     position = \"front\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@74 {
   target-path = \"/tegra-camera-platform/modules/module1/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 30-001c\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_b@1c\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module2_r325 = """
 fragment@75 {
   target-path = \"/tegra-camera-platform/modules/module2\";
   __overlay__ {
     badge = \"isx021_topright\";
     position = \"topright\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@76 {
   target-path = \"/tegra-camera-platform/modules/module2/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 31-001b\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_c@1b\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module3_r325 = """
 fragment@77 {
   target-path = \"/tegra-camera-platform/modules/module3\";
   __overlay__ {
     badge = \"isx021_bottomright\";
     position = \"bottomright\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@78 {
   target-path = \"/tegra-camera-platform/modules/module3/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 31-001c\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_d@1c\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module4_r325 = """
 fragment@79 {
   target-path = \"/tegra-camera-platform/modules/module4\";
   __overlay__ {
     badge = \"isx021_topleft\";
     position = \"topleft\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@80 {
   target-path = \"/tegra-camera-platform/modules/module4/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 32-001b\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_e@1b\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module5_r325 = """
 fragment@81 {
   target-path = \"/tegra-camera-platform/modules/module5\";
   __overlay__ {
     badge = \"isx021_centerright\";
     position = \"centerright\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@82 {
   target-path = \"/tegra-camera-platform/modules/module5/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 32-001c\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_f@1c\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module6_r325 = """
 fragment@83 {
   target-path = \"/tegra-camera-platform/modules/module6\";
   __overlay__ {
     badge = \"isx021_centerleft\";
     position = \"centerleft\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@84 {
   target-path = \"/tegra-camera-platform/modules/module6/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 33-001b\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/isx021_g@1b\";
   };
 };
"""

# -----------------------------------------------

str_fragment_isx021_camera_module7_r325 = """
 fragment@85{
   target-path = \"/tegra-camera-platform/modules/module7\";
   __overlay__ {
     badge = \"isx021_bottomleft\";
     position = \"bottomleft\";
     orientation = \"1\";
     status = \"okay\";
   };
 };

 fragment@86 {
   target-path = \"/tegra-camera-platform/modules/module7/drivernode0\";
   __overlay__ {
     status = \"okay\";
     pcl_id = \"v4l2_sensor\";
     devname = \"isx021 33-001c\";
     proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/isx021_h@1c\";
   };
 };
"""

# ============ CAMERA MODULES  R35.4.1 ============

str_fragment_camera_module_r3541 = """

// ----- Camera modules -----

  fragment@70 {
    //tcp
    target-path = "/tegra-camera-platform\";
    __overlay__ {
      status = \"okay\";
      num_csi_lanes = <0x02>;
      max_lane_speed = <4000000>;
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module0_r3541 = """
  fragment@71 {
    target-path = \"/tegra-camera-platform/modules/module0\";
    __overlay__ {
      badge = \"isx021_rear\";
      position = \"rear\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@72 {
    target-path = \"/tegra-camera-platform/modules/module0/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 30-001b\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_a@1b\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module1_r3541 = """
  fragment@73 {
    target-path = \"/tegra-camera-platform/modules/module1\";
    __overlay__ {
      badge = \"isx021_front\";
      position = \"front\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@74 {
    target-path = \"/tegra-camera-platform/modules/module1/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 30-001c\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_b@1c\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module2_r3541 = """
  fragment@75 {
    target-path = \"/tegra-camera-platform/modules/module2\";
    __overlay__ {
      badge = \"isx021_topright\";
      position = \"topright\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@76 {
    target-path = \"/tegra-camera-platform/modules/module2/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 31-001b\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_c@1b\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module3_r3541 = """
  fragment@77 {
    target-path = \"/tegra-camera-platform/modules/module3\";
    __overlay__ {
      badge = \"isx021_bottomright\";
      position = \"bottomright\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@78 {
    target-path = \"/tegra-camera-platform/modules/module3/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 31-001c\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_d@1c\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module4_r3541 = """
  fragment@79 {
    target-path = \"/tegra-camera-platform/modules/module4\";
    __overlay__ {
      badge = \"isx021_topleft\";
      position = \"topleft\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@80 {
    target-path = \"/tegra-camera-platform/modules/module4/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 32-001b\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_e@1b\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module5_r3541 = """
  fragment@81 {
    target-path = \"/tegra-camera-platform/modules/module5\";
    __overlay__ {
      badge = \"isx021_centerright\";
      position = \"centerright\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@82 {
    target-path = \"/tegra-camera-platform/modules/module5/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 32-001c\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_f@1c\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module6_r3541 = """
  fragment@83 {
    target-path = \"/tegra-camera-platform/modules/module6\";
    __overlay__ {
      badge = \"isx021_centerleft\";
      position = \"centerleft\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@84 {
    target-path = \"/tegra-camera-platform/modules/module6/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 33-001b\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_g@1b\";
    };
  };
"""

# -----------------------------------------------

str_fragment_camera_module7_r3541 = """
  fragment@85 {
    target-path = \"/tegra-camera-platform/modules/module7\";
    __overlay__ {
      badge = \"isx021_bottomleftt\";
      position = \"bottomleft\";
      orientation = \"1\";
      status = \"okay\";
    };
  };

  fragment@86 {
    target-path = \"/tegra-camera-platform/modules/module6/drivernode0\";
    __overlay__ {
      status = \"okay\";
      pcl_id = \"v4l2_sensor\";
      devname = \"isx021 33-001c\";
      proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_h@1c\";
    };
  };
"""

dict_fragment_camera_module = {
    "325x": str_fragment_camera_module_r325,
    "3541": str_fragment_camera_module_r3541,
}

dict_fragment_isx021_camera_module0 = {
    "325x": str_fragment_isx021_camera_module0_r325,
    "3541": str_fragment_camera_module0_r3541,
}
dict_fragment_isx021_camera_module1 = {
    "325x": str_fragment_isx021_camera_module1_r325,
    "3541": str_fragment_camera_module1_r3541,
}
dict_fragment_isx021_camera_module2 = {
    "325x": str_fragment_isx021_camera_module2_r325,
    "3541": str_fragment_camera_module2_r3541,
}
dict_fragment_isx021_camera_module3 = {
    "325x": str_fragment_isx021_camera_module3_r325,
    "3541": str_fragment_camera_module3_r3541,
}
dict_fragment_isx021_camera_module4 = {
    "325x": str_fragment_isx021_camera_module4_r325,
    "3541": str_fragment_camera_module4_r3541,
}
dict_fragment_isx021_camera_module5 = {
    "325x": str_fragment_isx021_camera_module5_r325,
    "3541": str_fragment_camera_module5_r3541,
}
dict_fragment_isx021_camera_module6 = {
    "325x": str_fragment_isx021_camera_module6_r325,
    "3541": str_fragment_camera_module6_r3541,
}
dict_fragment_isx021_camera_module7 = {
    "325x": str_fragment_isx021_camera_module7_r325,
    "3541": str_fragment_camera_module7_r3541,
}

# ===================  I2C ======================

str_fragment_i2c = """
// -----  I2C client  -----

  fragment@90{
    //cami2c/i2c@0
    target-path = \"/i2c@c240000/tca9546@70\";
    __overlay__ {
      status = \"okay\";
    };
  };
"""

# -----------------------------------------------

str_fragment_i2c_n = """
  fragment@91{
    //cami2c/i2c@0
    target-path = \"/i2c@c240000/tca9546@70/i2c@0\";
    __overlay__ {
"""

str_fragment_i2c_0 = str_fragment_i2c_n
str_fragment_i2c_1 = str_fragment_i2c_n.replace("@91", "@92").replace("c@0", "c@1")
str_fragment_i2c_2 = str_fragment_i2c_n.replace("@91", "@93").replace("c@0", "c@2")
str_fragment_i2c_3 = str_fragment_i2c_n.replace("@91", "@94").replace("c@0", "c@3")

# ==================   DSER   ====================

#  ---------- for R32.5 ----------------------------

str_fragment_dser_n = """

  fragment@95{
    target = <&dser>;
    __overlay__ {
      compatible = \"nvidia,tier4_max9296\";
      reg = <0x48>;
      status = \"okay\";
      csi-mode = \"2x4\";
      max-src = <2>;
      //reset-gpios = <&tegra_main_gpio CAM0_PWDN GPIO_ACTIVE_HIGH>;
      //reset-gpios = <&tegra_main_gpio 0x3E 0x0>;
    };
  };

"""

str_fragment_dser_0_r325 = str_fragment_dser_n
str_fragment_dser_1_r325 = str_fragment_dser_n \
    .replace("fragment@95", "fragment@96") \
    .replace("&dser", "&dsera")
str_fragment_dser_2_r325 = str_fragment_dser_n \
    .replace("fragment@95", "fragment@97") \
    .replace("&dser", "&dserb")
str_fragment_dser_3_r325 = str_fragment_dser_n \
    .replace("fragment@95", "fragment@98") \
    .replace("&dser", "&dserc")

str_fragment_dser_0 = {"325x": str_fragment_dser_0_r325, "3541": ""}
str_fragment_dser_1 = {"325x": str_fragment_dser_1_r325, "3541": ""}
str_fragment_dser_2 = {"325x": str_fragment_dser_2_r325, "3541": ""}
str_fragment_dser_3 = {"325x": str_fragment_dser_3_r325, "3541": ""}


#  ---------- for R35.4.1 ----------------------------

str_i2c_dser_n = """
      dser_0: max9296@48 {
        compatible = \"nvidia,tier4_max9296\";
        reg = <0x48>;
        status = \"okay\";
        csi-mode = \"2x4\";
        max-src = <2>;
      };
"""
str_i2c_dser_0_r3541 = str_i2c_dser_n
str_i2c_dser_1_r3541 = str_i2c_dser_n.replace("dser_0:", "dser_1:")
str_i2c_dser_2_r3541 = str_i2c_dser_n.replace("dser_0:", "dser_2:")
str_i2c_dser_3_r3541 = str_i2c_dser_n.replace("dser_0:", "dser_3:")

str_i2c_dser_0 = {"325x": "", "3541": str_i2c_dser_0_r3541}
str_i2c_dser_1 = {"325x": "", "3541": str_i2c_dser_1_r3541}
str_i2c_dser_2 = {"325x": "", "3541": str_i2c_dser_2_r3541}
str_i2c_dser_3 = {"325x": "", "3541": str_i2c_dser_3_r3541}

# ==================   SER   ====================

str_i2c_ser_n = """
      max9295_prim@62 {
        compatible = \"nvidia,tier4_max9295\";
        status = \"okay\";
        reg = <0x62>;
        is-prim-ser;
      };
      max9295_ser_a: max9295_a@42 {
        compatible = \"nvidia,tier4_max9295\";
        reg = <0x42>;
        nvidia,gmsl-dser-device = <&dser>;
      };

      max9295_ser_b: max9295_b@60 {
        compatible = \"nvidia,tier4_max9295\";
        status = \"okay\";
        reg = <0x60>;
        nvidia,gmsl-dser-device = <&dser>;
      };
"""

str_i2c_ser_0_r325 = str_i2c_ser_n

str_i2c_ser_1_r325 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_b_1")
    .replace("max9295_ser_a", "max9295_ser_b_0")
    .replace("&dser", "&dsera")
)
str_i2c_ser_2_r325 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_c_1")
    .replace("max9295_ser_a", "max9295_ser_c_0")
    .replace("&dser", "&dserb")
)
str_i2c_ser_3_r325 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_d_1")
    .replace("max9295_ser_a", "max9295_ser_d_0")
    .replace("&dser", "&dserc")
)

#  ---------- for R35.4.1 ----------------------------

str_i2c_ser_0_r3541 = str_i2c_ser_n.replace("&dser", "&dser_0")

str_i2c_ser_1_r3541 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_b_1")
    .replace("max9295_ser_a", "max9295_ser_b_0")
    .replace("&dser", "&dser_1")
)
str_i2c_ser_2_r3541 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_c_1")
    .replace("max9295_ser_a", "max9295_ser_c_0")
    .replace("&dser", "&dser_2")
)
str_i2c_ser_3_r3541 = (
    str_i2c_ser_n.replace("max9295_ser_b", "max9295_ser_d_1")
    .replace("max9295_ser_a", "max9295_ser_d_0")
    .replace("&dser", "&dser_3")
)

str_i2c_ser_0 = {"325x": str_i2c_ser_0_r325, "3541": str_i2c_ser_0_r3541}
str_i2c_ser_1 = {"325x": str_i2c_ser_1_r325, "3541": str_i2c_ser_1_r3541}
str_i2c_ser_2 = {"325x": str_i2c_ser_2_r325, "3541": str_i2c_ser_2_r3541}
str_i2c_ser_3 = {"325x": str_i2c_ser_3_r325, "3541": str_i2c_ser_3_r3541}


# ==================  ISX021  ===================

str_i2c_isx021_n_p1 = """
      isx021_a@1b {
        //cvb
        status = \"okay\";
        def-addr = <0x1a>;
        mclk = \"extperiph1\";
        clocks = <&bpmp 36U>, <&bpmp 36U>;
        clock-names = \"extperiph1\", \"pllp_grtba\";
        nvidia,gmsl-ser-device = <&max9295_ser_a>;
        nvidia,gmsl-dser-device = <&dser>;
        nvidia,fpga-device  = <&t4_fpga>;

        // common modul;e
        compatible = \"nvidia,tier4_isx021\";
        reg = <0x1b>;

        /* Physical dimensions of sensor */
        physical_w = \"15.0\";
        physical_h = \"12.5\";
        reg_mux = <0>;
"""

# ----------------  For R32.5 ------------------

str_i2c_isx021_0_p1_r325 = str_i2c_isx021_n_p1

str_i2c_isx021_1_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_b@1c")
    .replace("max9295_ser_a", "max9295_ser_b")
    .replace("reg = <0x1b>", "reg = <0x1c>")
)
str_i2c_isx021_2_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_c@1b")
    .replace("max9295_ser_a", "max9295_ser_b_0")
    .replace("&dser", "&dsera")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_isx021_3_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_d@1c")
    .replace("max9295_ser_a", "max9295_ser_b_1")
    .replace("&dser", "&dsera")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace(" reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_isx021_4_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_e@1b")
    .replace("max9295_ser_a", "max9295_ser_c_0")
    .replace("&dser", "&dserb")
    .replace("reg = <0x1b>", "reg = <0x1b>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_isx021_5_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_f@1c")
    .replace("max9295_ser_a", "max9295_ser_c_1")
    .replace("&dser", "&dserb")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_isx021_6_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_g@1b")
    .replace("max9295_ser_a", "max9295_ser_d_0")
    .replace("&dser", "&dserc")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)
str_i2c_isx021_7_p1_r325 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_h@1c")
    .replace("max9295_ser_a", "max9295_ser_d_1")
    .replace("&dser", "&dserc")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)

# ----------------  For R35.4.1 ------------------

str_i2c_isx021_0_p1_r3541 = str_i2c_isx021_n_p1.replace("&dser", "&dser_0")

str_i2c_isx021_1_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_b@1c")
    .replace("max9295_ser_a", "max9295_ser_b")
     .replace("&dser", "&dser_0")
    .replace("reg = <0x1b>", "reg = <0x1c>")
)
str_i2c_isx021_2_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_c@1b")
    .replace("max9295_ser_a", "max9295_ser_b_0")
    .replace("&dser", "&dser_1")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_isx021_3_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_d@1c")
    .replace("max9295_ser_a", "max9295_ser_b_1")
    .replace("&dser", "&dser_1")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace(" reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_isx021_4_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_e@1b")
    .replace("max9295_ser_a", "max9295_ser_c_0")
    .replace("&dser", "&dser_2")
    .replace("reg = <0x1b>", "reg = <0x1b>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_isx021_5_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_f@1c")
    .replace("max9295_ser_a", "max9295_ser_c_1")
    .replace("&dser", "&dser_2")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_isx021_6_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_g@1b")
    .replace("max9295_ser_a", "max9295_ser_d_0")
    .replace("&dser", "&dser_3")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)
str_i2c_isx021_7_p1_r3541 = (
    str_i2c_isx021_n_p1.replace("isx021_a@1b", "isx021_h@1c")
    .replace("max9295_ser_a", "max9295_ser_d_1")
    .replace("&dser", "&dser_3")
    .replace("reg = <0x1b>", "reg = <0x1c>")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)

str_i2c_isx021_0_p1 = {"325x": str_i2c_isx021_0_p1_r325, "3541": str_i2c_isx021_0_p1_r3541}
str_i2c_isx021_1_p1 = {"325x": str_i2c_isx021_1_p1_r325, "3541": str_i2c_isx021_1_p1_r3541}
str_i2c_isx021_2_p1 = {"325x": str_i2c_isx021_2_p1_r325, "3541": str_i2c_isx021_2_p1_r3541}
str_i2c_isx021_3_p1 = {"325x": str_i2c_isx021_3_p1_r325, "3541": str_i2c_isx021_3_p1_r3541}
str_i2c_isx021_4_p1 = {"325x": str_i2c_isx021_4_p1_r325, "3541": str_i2c_isx021_4_p1_r3541}
str_i2c_isx021_5_p1 = {"325x": str_i2c_isx021_5_p1_r325, "3541": str_i2c_isx021_5_p1_r3541}
str_i2c_isx021_6_p1 = {"325x": str_i2c_isx021_6_p1_r325, "3541": str_i2c_isx021_6_p1_r3541}
str_i2c_isx021_7_p1 = {"325x": str_i2c_isx021_7_p1_r325, "3541": str_i2c_isx021_7_p1_r3541}

# -----------------------------------------------

str_i2c_isx021_n_p2 = """
        sensor_model =\"isx021\";

        fsync-mode = \"false\";

        distortion-correction = \"false\";

        auto-exposure = \"true\";

        /* Defines number of frames to be dropped by driver internally after applying */
        /* sensor crop settings. Some sensors send corrupt frames after applying */
        /* crop co-ordinates */
        post_crop_frame_drop = \"0\";

        /* Convert Gain to unit of dB (decibel) befor passing to kernel driver */
        use_decibel_gain = \"true\";

        /* enable CID_SENSOR_MODE_ID for sensor modes selection */
        //use_sensor_mode_id = \"true\";
        use_sensor_mode_id = \"false\";

        mode0 {
          /*mode ISX021_MODE_1920X1280_CROP_30FPS*/
          mclk_khz = \"24000\";
          num_lanes = \"2\";
          tegra_sinterface = \"serial_a\";
          vc_id = \"0\";
          discontinuous_clk = \"no\";
          dpcm_enable = \"false\";
          cil_settletime = \"0\";
          dynamic_pixel_bit_depth = \"16\";
          csi_pixel_bit_depth = \"16\";
          mode_type = \"yuv\";
          pixel_phase = \"uyvy\";

          active_w = \"1920\";
          active_h = \"1280\";
          readout_orientation = \"0\";
          line_length = \"2250\";
          inherent_gain = \"1\";

          pix_clk_hz = \"94500000\";
          serdes_pix_clk_hz = \"833333333\";

          gain_factor = \"10\";
          min_gain_val = \"0\";                   /* dB */
          max_gain_val = \"300\";                 /* dB */
          step_gain_val = \"3\";                  /* 0.3 */
          default_gain = \"0\";
          min_hdr_ratio = \"1\";
          max_hdr_ratio = \"1\";
          framerate_factor = \"1000000\";
          min_framerate = \"30000000\";
          max_framerate = \"30000000\";
          step_framerate = \"1\";
          default_framerate = \"30000000\";
          exposure_factor = \"1000000\";
          min_exp_time = \"24\";                  /* us 1 line */
          max_exp_time = \"33333\";
          step_exp_time = \"1\";
          default_exp_time = \"33333\";           /* us */
          embedded_metadata_height = \"0\";
        };

        mode1 {
          /*mode ISX021_MODE_1920X1280_CROP_30FPS with Front Embedded data*/
          mclk_khz = \"24000\";
          num_lanes = \"4\";
          tegra_sinterface = \"serial_a\";
          vc_id = \"0\";
          discontinuous_clk = \"no\";
          dpcm_enable = \"false\";
          cil_settletime = \"0\";
          dynamic_pixel_bit_depth = \"16\";
          csi_pixel_bit_depth = \"16\";
          mode_type = \"yuv\";
          pixel_phase = \"uyvy\";

          active_w = \"1920\";
          active_h = \"1281\";
          readout_orientation = \"0\";
          line_length = \"2250\";
          inherent_gain = \"1\";

          pix_clk_hz = \"94500000\";
          serdes_pix_clk_hz = \"833333333\";

          gain_factor = \"10\";
          min_gain_val = \"0\";                   /* dB */
          max_gain_val = \"300\";                 /* dB */
          step_gain_val = \"3\";                  /* 0.3 */
          default_gain = \"0\";
          min_hdr_ratio = \"1\";
          max_hdr_ratio = \"1\";
          framerate_factor = \"1000000\";
          min_framerate = \"30000000\";
          max_framerate = \"30000000\";
          step_framerate = \"1\";
          default_framerate = \"30000000\";
          exposure_factor = \"1000000\";
          min_exp_time = \"24\";                  /* us 1 line */
          max_exp_time = \"33333\";
          step_exp_time = \"1\";
          default_exp_time = \"33333\";           /* us */
          embedded_metadata_height = \"0\";
        };

        mode2 {
          /*mode ISX021_MODE_1920X1280_CROP_30FPS with Rear Embedded data*/
          mclk_khz = \"24000\";
          num_lanes = \"4\";
          tegra_sinterface = \"serial_a\";
          vc_id = \"0\";
          discontinuous_clk = \"no\";
          dpcm_enable = \"false\";
          cil_settletime = \"0\";
          dynamic_pixel_bit_depth = \"16\";
          csi_pixel_bit_depth = \"16\";
          mode_type = \"yuv\";
          pixel_phase = \"uyvy\";

          active_w = \"1920\";
          active_h = \"1294\";
          readout_orientation = \"0\";
          line_length = \"2250\";
          inherent_gain = \"1\";

          pix_clk_hz = \"94500000\";
          serdes_pix_clk_hz = \"833333333\";

          gain_factor = \"10\";
          min_gain_val = \"0\";                   /* dB */
          max_gain_val = \"300\";                 /* dB */
          step_gain_val = \"3\";                  /* 0.3 */
          default_gain = \"0\";
          min_hdr_ratio = \"1\";
          max_hdr_ratio = \"1\";
          framerate_factor = \"1000000\";
          min_framerate = \"30000000\";
          max_framerate = \"30000000\";
          step_framerate = \"1\";
          default_framerate = \"30000000\";
          exposure_factor = \"1000000\";
          min_exp_time = \"24\";                  /* us 1 line */
          max_exp_time = \"33333\";
          step_exp_time = \"1\";
          default_exp_time = \"33333\";           /* us */
          embedded_metadata_height = \"0\";
        };

        mode3 {
          /*mode ISX021_MODE_1920X1280_CROP_30FPS with Front and Rear Embedded data*/
          mclk_khz = \"24000\";
          num_lanes = \"4\";
          tegra_sinterface = \"serial_a\";
          vc_id = \"0\";
          discontinuous_clk = \"no\";
          dpcm_enable = \"false\";
          cil_settletime = \"0\";
          dynamic_pixel_bit_depth = \"16\";
          csi_pixel_bit_depth = \"16\";
          mode_type = \"yuv\";
          pixel_phase = \"uyvy\";

          active_w = \"1920\";
          active_h = \"1295\";
          readout_orientation = \"0\";
          line_length = \"2250\";
          inherent_gain = \"1\";

          pix_clk_hz = \"94500000\";
          serdes_pix_clk_hz = \"833333333\";

          gain_factor = \"10\";
          min_gain_val = \"0\";                   /* dB */
          max_gain_val = \"300\";                 /* dB */
          step_gain_val = \"3\";                  /* 0.3 */
          default_gain = \"0\";
          min_hdr_ratio = \"1\";
          max_hdr_ratio = \"1\";
          framerate_factor = \"1000000\";
          min_framerate = \"30000000\";
          max_framerate = \"30000000\";
          step_framerate = \"1\";
          default_framerate = \"30000000\";
          exposure_factor = \"1000000\";
          min_exp_time = \"24\";                  /* us 1 line */
          max_exp_time = \"33333\";
          step_exp_time = \"1\";
          default_exp_time = \"33333\";           /* us */
          embedded_metadata_height = \"0\";
        };
"""


str_i2c_isx021_0_p2 = str_i2c_isx021_n_p2
str_i2c_isx021_1_p2 = str_i2c_isx021_n_p2.replace('vc_id = "0"', 'vc_id = "1"')
str_i2c_isx021_2_p2 = str_i2c_isx021_n_p2.replace("serial_a", "serial_c")
str_i2c_isx021_3_p2 = str_i2c_isx021_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_c"
)
str_i2c_isx021_4_p2 = str_i2c_isx021_n_p2.replace("serial_a", "serial_e")
str_i2c_isx021_5_p2 = str_i2c_isx021_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_e"
)
str_i2c_isx021_6_p2 = str_i2c_isx021_n_p2.replace("serial_a", "serial_g")
str_i2c_isx021_7_p2 = str_i2c_isx021_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_g"
)

# -----------------------------------------------

str_i2c_isx021_n_p3 = """
        ports {
          #address-cells = <1>;
          #size-cells = <0>;
          port@0 {
            reg = <0>;
            isx021_out0: endpoint {
              vc-id = <0>;
              port-index = <0>;
              bus-width = <2>;
              remote-endpoint = <&csi_in0>;
            };
          };
        };
"""

str_i2c_isx021_0_p3 = str_i2c_isx021_n_p3
str_i2c_isx021_1_p3 = (
    str_i2c_isx021_n_p3.replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in1")
    .replace("isx021_out0", "isx021_out1")
)
str_i2c_isx021_2_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <2>")
    .replace("csi_in0", "csi_in2")
    .replace("isx021_out0", "isx021_out2")
)
str_i2c_isx021_3_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <2>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in3")
    .replace("isx021_out0", "isx021_out3")
)
str_i2c_isx021_4_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <4>")
    .replace("csi_in0", "csi_in4")
    .replace("isx021_out0", "isx021_out4")
)
str_i2c_isx021_5_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <4>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in5")
    .replace("isx021_out0", "isx021_out5")
)
str_i2c_isx021_6_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <6>")
    .replace("csi_in0", "csi_in6")
    .replace("isx021_out0", "isx021_out6")
)
str_i2c_isx021_7_p3 = (
    str_i2c_isx021_n_p3.replace("port-index = <0>", "port-index = <6>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in7")
    .replace("isx021_out0", "isx021_out7")
)

str_i2c_isx021_n_p4 = """
        gmsl-link {
          src-csi-port = \"b\";           /* Port at which sensor is connected to its serializer device. */
          dst-csi-port = \"a\";           /* Destination CSI port on the Jetson side, connected at deserializer. */
          serdes-csi-link = \"a\";        /* GMSL link sensor/serializer connected */
          csi-mode = \"1x4\";             /*  to sensor CSI mode. */
          st-vc = <0>;                  /* Sensor source default VC ID: 0 unless overridden by sensor. */
          vc-id = <0>;                  /* Destination VC ID, assigned to sensor stream by deserializer. */
          num-lanes = <2>;              /* Number of CSI lanes used. */
          streams = \"ued-u1\", \"yuv8\";   /* Types of streams sensor is streaming. */
        };
      };"""


str_i2c_isx021_0_p4 = str_i2c_isx021_n_p4
str_i2c_isx021_1_p4 = str_i2c_isx021_n_p4.replace(
    'serdes-csi-link = "a"', ' serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_isx021_2_p4 = str_i2c_isx021_n_p4
str_i2c_isx021_3_p4 = str_i2c_isx021_n_p4.replace(
    'serdes-csi-link = "a"', ' serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_isx021_4_p4 = str_i2c_isx021_n_p4
str_i2c_isx021_5_p4 = str_i2c_isx021_n_p4.replace(
    'serdes-csi-link = "a"', ' serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_isx021_6_p4 = str_i2c_isx021_n_p4
str_i2c_isx021_7_p4 = str_i2c_isx021_n_p4.replace(
    'serdes-csi-link = "a"', ' serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")

# ==================  ISP  ======================

str_i2c_isp_n = """
      gw5300_prim@6d {
          compatible = \"nvidia,tier4_gw5300\";
          reg = <0x6d>;
          is-prim-isp;
      };

      isp_a: gw5300_a@6e {
          compatible = \"nvidia,tier4_gw5300\";
          reg = <0x6e>;
      };

      isp_b: gw5300_b@6f {
          compatible = \"nvidia,tier4_gw5300\";
          reg = <0x6f>;
      };
"""

#str_i2c_isp_0 = str_i2c_isp_n

#str_i2c_isp_1 = str_i2c_isp_n.replace("isp_a", "isp_c").replace("isp_b", "isp_d")
#str_i2c_isp_2 = str_i2c_isp_n.replace("isp_a", "isp_e").replace("isp_b", "isp_f")
#str_i2c_isp_3 = str_i2c_isp_n.replace("isp_a", "isp_g").replace("isp_b", "isp_h")

str_i2c_0_isp = str_i2c_isp_n
str_i2c_1_isp = str_i2c_isp_n.replace("isp_a", "isp_c").replace("isp_b", "isp_d")
str_i2c_2_isp = str_i2c_isp_n.replace("isp_a", "isp_e").replace("isp_b", "isp_f")
str_i2c_3_isp = str_i2c_isp_n.replace("isp_a", "isp_g").replace("isp_b", "isp_h")

# ==================  IMX490  ===================

str_i2c_imx490_n_p1 = """
      imx490_a@2b {
        compatible = \"nvidia,tier4_imx490\";
        def-addr = <0x1a>;
        // clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
        clock-names = \"extperiph1\", \"pllp_grtba\";
        mclk = \"extperiph1\";
        nvidia,isp-device = <&isp_a>;           // for C2 camera
        nvidia,gmsl-ser-device = <&max9295_ser_a>;
        nvidia,gmsl-dser-device = <&dser>;
        nvidia,fpga-device  = <&t4_fpga>;

        reg = <0x2b>;

        /* Physical dimensions of sensor */
        physical_w = \"15.0\";
        physical_h = \"12.5\";
        reg_mux = <0>;
"""

# -------------  For R32.5 ---------------------

str_i2c_imx490_0_p1_r325 = str_i2c_imx490_n_p1

str_i2c_imx490_1_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_b@2c")
    .replace("&max9295_ser_a", "&max9295_ser_b")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("isp_a", "isp_b")
)
str_i2c_imx490_2_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_c@2b")
    .replace("&max9295_ser_a", "&max9295_ser_b_0")
    .replace("isp_a", "isp_c")
    .replace("&dser", "&dsera")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_imx490_3_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_d@2c")
    .replace("&max9295_ser_a", "&max9295_ser_b_1")
    .replace("isp_a", "isp_d")
    .replace("&dser", "&dsera")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_imx490_4_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_e@2b")
    .replace("&max9295_ser_a", "&max9295_ser_c_0")
    .replace("isp_a", "isp_e")
    .replace("&dser", "&dserb")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_imx490_5_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_f@2c")
    .replace("&max9295_ser_a", "&max9295_ser_c_1")
    .replace("isp_a", "isp_f")
    .replace("&dser", "&dserb")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_imx490_6_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_g@2b")
    .replace("&max9295_ser_a", "&max9295_ser_d_0")
    .replace("isp_a", "isp_g")
    .replace("&dser", "&dserc")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)
str_i2c_imx490_7_p1_r325 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_h@2c")
    .replace("&max9295_ser_a", "&max9295_ser_d_1")
    .replace("isp_a", "isp_h")
    .replace("&dser", "&dserc")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)

# -------------   For R35.4.1 ---------------------

str_i2c_imx490_0_p1_r3541 = str_i2c_imx490_n_p1.replace("&dser", "&dser_0")

str_i2c_imx490_1_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_b@2c")
    .replace("&max9295_ser_a", "&max9295_ser_b")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("&dser", "&dser_0")
    .replace("isp_a", "isp_b")
)
str_i2c_imx490_2_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_c@2b")
    .replace("&max9295_ser_a", "&max9295_ser_b_0")
    .replace("isp_a", "isp_c")
    .replace("&dser", "&dser_1")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_imx490_3_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_d@2c")
    .replace("&max9295_ser_a", "&max9295_ser_b_1")
    .replace("isp_a", "isp_d")
    .replace("&dser", "&dser_1")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <1>")
)
str_i2c_imx490_4_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_e@2b")
    .replace("&max9295_ser_a", "&max9295_ser_c_0")
    .replace("isp_a", "isp_e")
    .replace("&dser", "&dser_2")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_imx490_5_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_f@2c")
    .replace("&max9295_ser_a", "&max9295_ser_c_1")
    .replace("isp_a", "isp_f")
    .replace("&dser", "&dser_2")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <2>")
)
str_i2c_imx490_6_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_g@2b")
    .replace("&max9295_ser_a", "&max9295_ser_d_0")
    .replace("isp_a", "isp_g")
    .replace("&dser", "&dser_3")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)
str_i2c_imx490_7_p1_r3541 = (
    str_i2c_imx490_n_p1.replace("imx490_a@2b", "imx490_h@2c")
    .replace("&max9295_ser_a", "&max9295_ser_d_1")
    .replace("isp_a", "isp_h")
    .replace("&dser", "&dser_3")
    .replace("reg = <0x2b>", "reg = <0x2c>")
    .replace("reg_mux = <0>", "reg_mux = <3>")
)

str_i2c_imx490_0_p1 = {"325x": str_i2c_imx490_0_p1_r325, "3541": str_i2c_imx490_0_p1_r3541}
str_i2c_imx490_1_p1 = {"325x": str_i2c_imx490_1_p1_r325, "3541": str_i2c_imx490_1_p1_r3541}
str_i2c_imx490_2_p1 = {"325x": str_i2c_imx490_2_p1_r325, "3541": str_i2c_imx490_2_p1_r3541}
str_i2c_imx490_3_p1 = {"325x": str_i2c_imx490_3_p1_r325, "3541": str_i2c_imx490_3_p1_r3541}
str_i2c_imx490_4_p1 = {"325x": str_i2c_imx490_4_p1_r325, "3541": str_i2c_imx490_4_p1_r3541}
str_i2c_imx490_5_p1 = {"325x": str_i2c_imx490_5_p1_r325, "3541": str_i2c_imx490_5_p1_r3541}
str_i2c_imx490_6_p1 = {"325x": str_i2c_imx490_6_p1_r325, "3541": str_i2c_imx490_6_p1_r3541}
str_i2c_imx490_7_p1 = {"325x": str_i2c_imx490_7_p1_r325, "3541": str_i2c_imx490_7_p1_r3541}

# -----------------------------------------------

str_i2c_imx490_n_p2 = """

        sensor_model =\"imx490\";

        fsync-mode = \"false\";

        distortion-correction = \"false\";

        auto-exposure = \"true\";

        /* Defines number of frames to be dropped by driver internally after applying */
        /* sensor crop settings. Some sensors send corrupt frames after applying */
        /* crop co-ordinates */
        post_crop_frame_drop = \"0\";

        /* Convert Gain to unit of dB (decibel) befor passing to kernel driver */
        use_decibel_gain = \"true\";

        /* enable CID_SENSOR_MODE_ID for sensor modes selection */
        use_sensor_mode_id = \"true\";

        mode0 {/*mode IMX490_MODE_2880X1860_CROP_30FPS*/
          mclk_khz = \"24000\";
          num_lanes = \"2\";
          tegra_sinterface = \"serial_a\";
          vc_id = \"0\";
          discontinuous_clk = \"no\";
          dpcm_enable = \"false\";
          cil_settletime = \"0\";
          dynamic_pixel_bit_depth = \"16\";
          csi_pixel_bit_depth = \"16\";
          mode_type = \"yuv\";
          pixel_phase = \"uyvy\";
        
          active_w = \"2880\";
          active_h = \"1860\";
          readout_orientation = \"0\";
          line_length = \"2250\";
          inherent_gain = \"1\";
          pix_clk_hz = \"160704000\";
          serdes_pix_clk_hz = \"1200000000\";
        
          gain_factor = \"5\";
          min_gain_val = \"0\";                         /* dB */
          max_gain_val = \"300\";                       /* dB */
          step_gain_val = \"1\";                        /* 0.3 */
          default_gain = \"0\";
          min_hdr_ratio = \"1\";
          max_hdr_ratio = \"1\";
          framerate_factor = \"1000000\";
          min_framerate = \"0\";
          max_framerate = \"40954095\";
          step_framerate = \"1\";
          default_framerate = \"0\";
          exposure_factor = \"1000000\";
          min_exp_time = \"0\";                         /* us 1 line */
          max_exp_time = \"40954095\";
          step_exp_time = \"1\";
          default_exp_time = \"0\";                 /* us */
          embedded_metadata_height = \"0\";
        };
"""

str_i2c_imx490_0_p2 = str_i2c_imx490_n_p2
str_i2c_imx490_1_p2 = str_i2c_imx490_n_p2.replace('vc_id = "0"', 'vc_id = "1"')
str_i2c_imx490_2_p2 = str_i2c_imx490_n_p2.replace("serial_a", "serial_c")
str_i2c_imx490_3_p2 = str_i2c_imx490_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_c"
)
str_i2c_imx490_4_p2 = str_i2c_imx490_n_p2.replace("serial_a", "serial_e")
str_i2c_imx490_5_p2 = str_i2c_imx490_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_e"
)
str_i2c_imx490_6_p2 = str_i2c_imx490_n_p2.replace("serial_a", "serial_g")
str_i2c_imx490_7_p2 = str_i2c_imx490_n_p2.replace('vc_id = "0"', 'vc_id = "1"').replace(
    "serial_a", "serial_g"
)

# -----------------------------------------------

str_i2c_imx490_n_p3 = """
        ports {
          #address-cells = <1>;
          #size-cells = <0>;
          port@0 {
            reg = <0>;
            imx490_out0: endpoint {
              vc-id = <0>;
              port-index = <0>;
              bus-width = <2>;
              remote-endpoint = <&csi_in0>;
            };
          };
        };"""

str_i2c_imx490_0_p3 = str_i2c_imx490_n_p3
str_i2c_imx490_1_p3 = (
    str_i2c_imx490_n_p3.replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in1")
    .replace("imx490_out0", "imx490_out1")
)
str_i2c_imx490_2_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <2>")
    .replace("csi_in0", "csi_in2")
    .replace("imx490_out0", "imx490_out2")
)
str_i2c_imx490_3_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <2>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in3")
    .replace("imx490_out0", "imx490_out3")
)
str_i2c_imx490_4_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <4>")
    .replace("csi_in0", "csi_in4")
    .replace("imx490_out0", "imx490_out4")
)
str_i2c_imx490_5_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <4>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in5")
    .replace("imx490_out0", "imx490_out5")
)
str_i2c_imx490_6_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <6>")
    .replace("csi_in0", "csi_in6")
    .replace("imx490_out0", "imx490_out6")
)
str_i2c_imx490_7_p3 = (
    str_i2c_imx490_n_p3.replace("port-index = <0>", "port-index = <6>")
    .replace("vc-id = <0>", "vc-id = <1>")
    .replace("csi_in0", "csi_in7")
    .replace("imx490_out0", "imx490_out7")
)

# -----------------------------------------------

str_i2c_imx490_n_p4 = """
        gmsl-link {
          src-csi-port = \"b\";     /* Port at which sensor is connected to its serializer device. */
          dst-csi-port = \"a\";     /* Destination CSI port on the Jetson side, connected at deserializer. */
          serdes-csi-link = \"a\";  /* GMSL link sensor/serializer connected */
          csi-mode = \"1x4\";       /*  to sensor CSI mode. */
          st-vc = <0>;            /* Sensor source default VC ID: 0 unless overridden by sensor. */
          vc-id = <0>;            /* Destination VC ID, assigned to sensor stream by deserializer. */
          num-lanes = <2>;        /* Number of CSI lanes used. */
          streams = \"ued-u1\",\"yuv8\"; /* Types of streams sensor is streaming. */
        };
      };"""

str_i2c_imx490_0_p4 = str_i2c_imx490_n_p4
str_i2c_imx490_1_p4 = str_i2c_imx490_n_p4.replace(
    'serdes-csi-link = "a"', 'serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_imx490_2_p4 = str_i2c_imx490_n_p4
str_i2c_imx490_3_p4 = str_i2c_imx490_n_p4.replace(
    'serdes-csi-link = "a"', 'serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_imx490_4_p4 = str_i2c_imx490_n_p4
str_i2c_imx490_5_p4 = str_i2c_imx490_n_p4.replace(
    'serdes-csi-link = "a"', 'serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")
str_i2c_imx490_6_p4 = str_i2c_imx490_n_p4
str_i2c_imx490_7_p4 = str_i2c_imx490_n_p4.replace(
    'serdes-csi-link = "a"', 'serdes-csi-link = "b"'
).replace("vc-id = <0>", "vc-id = <1>")


# =============  DSER in Base DTB  ==============

str_fragment_dser_in_base_dtb_r3541 = """
      dser_0: max9296@48 {
        compatible = \"nvidia,tier4_max9296\";
        reg = <0x48>;
        status = \"okay\";
        csi-mode = \"2x4\";
        max-src = <2>;
      };
"""

str_fragment_dser_in_base_dtb_r325 = """

fragment@95{
      target = <&dser>;
      __overlay__ {
        compatible = "nvidia,tier4_max9296";
      };
    };

fragment@96{
      target = <&dsera>;
      __overlay__ {
        compatible = "nvidia,tier4_max9296";
      };
    };

fragment@97{
      target = <&dserb>;
      __overlay__ {
        compatible = "nvidia,tier4_max9296";
      };
    };

fragment@98{
      target = <&dserc>;
      __overlay__ {
        compatible = "nvidia,tier4_max9296";
      };
    };
"""

dict_fragment_dser_in_base_dtb = {
    "325x": str_fragment_dser_in_base_dtb_r325,
    "3541": str_fragment_dser_in_base_dtb_r3541,
}

# ===============   FPGA   ======================

str_fpga = """

// -----  FPGA -----

  fragment@100{
    target-path = \"/i2c@3180000\";
     __overlay__ {
       t4_fpga: tier4_fpga@66 {
       compatible = \"nvidia,tier4_fpga\";
       reg = <0x66>;
       status = \"okay\";
     };
   };
 };
"""

str_block_end = """
    };
  };"""

str_overlay_end = """
};
"""

# ===============================================
# ===========  Functions ========================
# ===============================================


def idenify_camera(cmd_arg):
    upper_arg = cmd_arg.upper()
    if upper_arg == "C1":
        return "C1"
    elif upper_arg == "C2":
        return "C2"
    else:
        return "NC"


# -----------------------------------------------


def get_type_of_cameras(l_camera):
    l_camera_type = [None] * 4

    for i in range(0, 4):
        k = 2 * i
        if l_camera[k] == "C1" or l_camera[k + 1] == "C1":
            l_camera_type[i] = "c1"
        elif l_camera[k] == "C2" or l_camera[k + 1] == "C2":
            l_camera_type[i] = "c2"
        elif l_camera[k] == "NC" and l_camera[k + 1] == "NC":
            l_camera_type[i] = "nc"

    return l_camera_type


# -----------------------------------------------


def build_symlink_c1c2(dev_num, v_num, camera_type):
    video_num = v_num + 1

    str_subsys = 'SUBSYSTEM=="video4linux", ATTR{name}=="vi-output, '
    str_symlink = 'SYMLINK+= "gmsl/tier4-isx021-imx490-cam' + str(video_num) + '"\n'

    if camera_type == "C1":
        driver_name = [
            'tier4_isx021 30-001b", ',
            'tier4_isx021 30-001c", ',
            'tier4_isx021 31-001b", ',
            'tier4_isx021 31-001c", ',
            'tier4_isx021 32-001b", ',
            'tier4_isx021 32-001c", ',
            'tier4_isx021 33-001b", ',
            'tier4_isx021 33-001c", ',
        ]

    elif camera_type == "C2":
        driver_name = [
            'tier4_imx490 30-002b", ',
            'tier4_imx490 30-002c", ',
            'tier4_imx490 31-002b", ',
            'tier4_imx490 31-002c", ',
            'tier4_imx490 32-002b", ',
            'tier4_imx490 32-002c", ',
            'tier4_imx490 33-002b", ',
            'tier4_imx490 33-002c", ',
        ]

    str_symlink_dev = str_subsys + driver_name[v_num] + str_symlink

    return str_symlink_dev


# -----------------------------------------------


def get_des_number(port_num):  # port_num is 0 to 7
    even_port = port_num & 0x6

    if even_port == 0:
        rc = 12  # port 1,2
    elif even_port == 2:
        rc = 34  # port 3,4
    elif even_port == 4:
        rc = 56  # port 5,6
    elif even_port == 6:
        rc = 78  # port 7,8
    return rc

def usage():
    print("************************************************************************")
    print("**                                                                    **")
    print("**     This is the tool to generate overlay dts and udev file.        **")
    print("**                                                                    **")
    print("************************************************************************")
    print("**                                                                    **")
    print("** Usage                                                              **")
    print("** ~~~~~                                                              **")
    print("** Case 1.                                                            **")
    print("**                                                                    **")
    print("**  $> make_overaly_dts_roscube.py Rev Camera1 Camera2.. Camera8      **")
    print("**                                                                    **")
    print("**     Rev: L4T Revision [ R32.5.1 R32.5.2 |  R32.6.1 | R35.4.1 ]     **")
    print("**                                                                    **")
    print("**     CameraX : C1 or C2 ( The camera connected to portX )           **")
    print("**     if no camera connected, specify C1                             **")
    print("**                                                                    **")
    print("**     E.g.                                                           **")
    print("**    $> make_overaly_dts_roscube.py R35.4.1 C1 C1 C1 C1 C2 C2 C1 C1  **")
    print("**             ( Total number of cameras should be 8 )                **")
    print("** Case2:                                                             **")
    print("**                                                                    **")
    print("**  $> make_overaly_dts_roscube.py option                             **")
    print("**                                                                    **")
    print("**  option :  Rev -2,-4,-6,-8 [C1,C2]                                 **")
    print("**                                                                    **")
    print("**     where,    Rev: L4T Revision [ R35.4.1 ]                        **")
    print("**      ( -2,4, is peatable, total number of Ns of -N should be 8 )   **")
    print("**                                                                    **")
    print("**     E.g.                                                           **")
    print("**        1. All cameras are C1                                       **")
    print("**                                                                    **")
    print("**           $> make_overaly_dts_roscube.py R32.5.1 -8 C1             **")
    print("**        2.                                                          **")
    print("**          4 cameras(port 1 to port 4 ) are C1,                      **")
    print("**          2 cameras( port 5&6 ) are C2                              **")
    print("**          2 cameras( port 7&8 ) are C1                              **")
    print("**                                                                    **")
    print("**       $> make_overaly_dts_roscube.py R32.5.1 -4 C1 -2 C2 -2 C1     **")
    print("**                                                                    **")
    print("** The followings should not be specified.( Mixed above 2 cases )     **")
    print("**                                                                    **")
    print("**    [-2, -4, -6, -8] option before [C1/C2]                          **")
    print("**    and [C1/C2] without those options are mixed.                    **")
    print("**                                                                    **")
    print("**     E.g.                                                           **")
    print("**       $> make_overaly_dts_roscube.py R35.4.1 -4 C1 -2 C2 C1 C2     **")
    print("**                                                                    **")
    print("************************************************************************")

# -----------------------------------------------


def check_and_set_next_port(num, camera):
    if (num & 0x1) == 0:
        if camera[num] == "C1":
            if camera[num + 1] == "C1" or camera[num + 1] == "NC":
                return 0
            else:
                return -2
        elif camera[num] == "C2":
            if camera[num + 1] == "C2" or camera[num + 1] == "NC":
                return 0
            else:
                return -2
        elif camera[num] == "NC":
            if camera[num + 1] == "C2" or camera[num + 1] == "C1":
                camera[num] = camera[num + 1]
                return 0
            else:
                return -2
    else:
        return 0


# -----------------------------------------------


def check_and_set_last_port(num, camera):
    if num & 0x1:
        if camera[num] == "C1":
            if camera[num - 1] == "C1" or camera[num - 1] == "NC":
                return 0
            else:
                return -1
        elif camera[num] == "C2":
            if camera[num - 1] == "C2" or camera[num - 1] == "NC":
                return 0
            else:
                return -1
        elif camera[num] == "NC":
            if camera[num - 1] == "C2" or camera[num - 1] == "C1":
                camera[num] = camera[num - 1]
                return 0
            else:
                return -1
    else:
        return 0


# -----------------------------------------------


def identify_camera(cmd_arg):
    upper_arg = cmd_arg.upper()
    if upper_arg == "C1":
        return "C1"
    elif upper_arg == "C2":
        return "C2"
    else:
        return "NC"


def is_option(str_arg):
    if str_arg[0] == "-":
        return 1
    else:
        return 0


# -----------------------------------------------


def is_option(s_arg):
    if s_arg[0] == "-":
        return 1
    else:
        return 0


# -----------------------------------------------


def get_n_options(args, l_total_num_args):
    m = 0

    l_str_n_options = [""] * l_total_num_args

    for k in range(0, l_total_num_args):
        #    print('k = ', k)
        if is_option(args[k]) == 1:
            #      print('m = ', m)
            str_temp_num_of_cameras = str(args[k])
            l_str_n_options[m] = str_temp_num_of_cameras[1:] + "-" + args[k + 1]
            m += 1
        l_str_n_options[m] = "fin"

    return l_str_n_options


# -----------------------------------------------


def get_n_to_end(lst_args, n):
    l = len(lst_args)
    if n <= l:
        rc = lst_args[n:]
    else:
        rc[0] = "fail"
        print(
            " get_n_to_args : n larger than length of lst_args l ="
            + str(l)
            + "n ="
            + str(n)
        )
    return rc


# -----------------------------------------------


def deploy_n_options(str_n_options):
    l_camera = [None] * MAX_NUM_CAMERAS

    l_pos = 0

    for i in range(MAX_NUM_CAMERAS):
        if str_n_options[i] != "fin":
            w_str_n_options = str_n_options[i].split("-")
            if len(w_str_n_options) == 2:
                k = int(w_str_n_options[0])
                str_w_camera = w_str_n_options[1]
                for m in range(k):
                    l_camera[l_pos + m] = str_w_camera.upper()
                l_pos += k
            else:
                break
        else:
            break
    if l_pos != MAX_NUM_CAMERAS:
        s1 = (
            "Error!! : Ttotal number of Cameras is invalid(should be 8). Actual Number = "
            + str(l_pos)
            + "\n"
        )
        s2 = "          Or\n"
        s3 = "          Options (-2,-4,-6,-8) specification and no option specification are mixed in the arguments."
        l_camera[0] = s1
        print(s1 + s2 + s3)
    return l_camera


# -----------------------------------------------

dict_isx021_serdes_pix_clk = {
    "325x": 'serdes_pix_clk_hz = "833333333"',
    "3541": 'serdes_pix_clk_hz = "187500000"',
}
dict_imx490_serdes_pix_clk = {
    "325x": 'serdes_pix_clk_hz = "1200000000"',
    "3541": 'serdes_pix_clk_hz = "187500000"',
}


def get_serdes_pix_clk(str_revision, str_camera_type):
    if str_camera_type == "C1":
        str_rc = dict_isx021_serdes_pix_clk[str_revision]

    elif str_camera_type == "C2":
        str_rc = dict_imx490_serdes_pix_clk[str_revision]

    return str_rc


# ===============================================
# ===================  Main  ====================
# ===============================================

args = sys.argv

temp_cam = [None] * MAX_NUM_CAMERAS

total_num_args = len(args)

l4t_revision = args[1].upper()

if l4t_revision == "R32.5.1" or l4t_revision == "R32.5.2" or l4t_revision == "R32.6.1":
    str_rev_num = "325x"
elif l4t_revision == "R35.4.1":
    str_rev_num = "3541"
else:
    print(" Error!! : The first argument should be R32.5.1, R32.5.2, R32.6.1 or R35.4.1.")
    usage()
    sys.exit()

str_n_options = get_n_options(args, total_num_args)

if str_n_options[0] == "fin":
    for i in range(8):
        temp_cam[i] = identify_camera(args[i + 2])

elif str_n_options[0] == "fail":
    sys.exit()

else:
    temp_cam = deploy_n_options(str_n_options)
    if temp_cam[0] != "C1" and temp_cam[0] != "C2":
        sys.exit()

#print("temp_cam =", temp_cam)

found_camera = 0

camera = ["NC", "NC", "NC", "NC", "NC", "NC", "NC", "NC"]

if total_num_args > 10:
    print(" ***** Error! : " + args[0] + " should be from 2 to 8 arguments. *****\n")
    usage()
    sys.exit()

# Replacing for R35.4.1.
# This is due to the implementation of `min_gain_val` being -1 in L4T R35.4.1. 
if l4t_revision == "R35.4.1":
    str_i2c_isx021_0_p2 = str_i2c_isx021_0_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_1_p2 = str_i2c_isx021_1_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_2_p2 = str_i2c_isx021_2_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_3_p2 = str_i2c_isx021_3_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_4_p2 = str_i2c_isx021_4_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_5_p2 = str_i2c_isx021_5_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_6_p2 = str_i2c_isx021_6_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_isx021_7_p2 = str_i2c_isx021_7_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')

    str_i2c_imx490_0_p2 = str_i2c_imx490_0_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_1_p2 = str_i2c_imx490_1_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_2_p2 = str_i2c_imx490_2_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_3_p2 = str_i2c_imx490_3_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_4_p2 = str_i2c_imx490_4_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_5_p2 = str_i2c_imx490_5_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_6_p2 = str_i2c_imx490_6_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')
    str_i2c_imx490_7_p2 = str_i2c_imx490_7_p2.replace('min_gain_val = "0"', 'min_gain_val = "1"')

for i in range(MAX_NUM_CAMERAS):
    if i & 0x0 == 0:
        err = check_and_set_next_port(i, temp_cam)
    else:
        err = check_and_set_last_port(i, temp_cam)

    if err == -1:
        print(
            " ***** Error! : port["
            + str(i - 1)
            + "] should be "
            + temp_cam[i]
            + " or NC. *****\n"
        )
        usage()
        sys.exit()
    elif err == -2:
        print(
            " ***** Error! : port["
            + str(i + 1)
            + "] should be "
            + temp_cam[i]
            + " or NC. *****\n"
        )
        usage()
        sys.exit()
    else:
        camera[i] = temp_cam[i]

    if i == 0:
        str_fragment_vi0 = dict_fragment_vi_0[str_rev_num]
        str_w_fragment_nvcsi0 = dict_fragment_nvcsi_ch0[str_rev_num]
        str_w_camera_module0 = dict_fragment_isx021_camera_module0[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_0_p2 = str_i2c_isx021_0_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera1 = (
                str_i2c_isx021_0_p1[str_rev_num]
                + str_w_i2c_isx021_0_p2
                + str_i2c_isx021_0_p3
                + str_i2c_isx021_0_p4
            )
            str_i2c_0_isp = ""
            str_fragment_nvcsi0 = str_w_fragment_nvcsi0
            str_camera_module0 = str_w_camera_module0
        elif camera[i] == "C2":
            str_w_i2c_imx490_0_p2 = str_i2c_imx490_0_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera1 = (
                str_i2c_imx490_0_p1[str_rev_num]
                + str_w_i2c_imx490_0_p2
                + str_i2c_imx490_0_p3
                + str_i2c_imx490_0_p4
            )
            #str_i2c_0_isp = str_i2c_isp_0
            str_fragment_nvcsi0 = str_w_fragment_nvcsi0.replace(
                "isx021_out0", "imx490_out0"
            )
            str_camera_module0 = (
                str_w_camera_module0.replace("isx021", "imx490")
                .replace("001b", "002b")
                .replace("@1b", "@2b")
            )
        else:
            str_camera1 = ""
            str_fragment_vi0 = ""
            str_fragment_nvcsi0 = ""
            str_camera_module0 = ""

    elif i == 1:
        str_fragment_vi1 = dict_fragment_vi_1[str_rev_num]
        str_w_fragment_nvcsi1 = dict_fragment_nvcsi_ch1[str_rev_num]
        str_w_camera_module1 = dict_fragment_isx021_camera_module1[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_1_p2 = str_i2c_isx021_1_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera2 = (
                str_i2c_isx021_1_p1[str_rev_num]
                + str_w_i2c_isx021_1_p2
                + str_i2c_isx021_1_p3
                + str_i2c_isx021_1_p4
            )
            str_i2c_0_isp = ""
            str_fragment_nvcsi1 = str_w_fragment_nvcsi1
            str_camera_module1 = str_w_camera_module1
        elif camera[i] == "C2":
            str_w_i2c_imx490_1_p2 = str_i2c_imx490_1_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera2 = (
                str_i2c_imx490_1_p1[str_rev_num]
                + str_w_i2c_imx490_1_p2
                + str_i2c_imx490_1_p3
                + str_i2c_imx490_1_p4
            )
            #str_i2c_0_isp = str_i2c_isp_0
            str_fragment_nvcsi1 = str_w_fragment_nvcsi1.replace(
                "isx021_out1", "imx490_out1"
            )
            str_camera_module1 = (
                str_w_camera_module1.replace("isx021", "imx490")
                .replace("001c", "002c")
                .replace("@1c", "@2c")
            )
        else:
            str_camera2 = ""
            str_fragment_vi1 = ""
            str_fragment_nvcsi1 = ""
            str_camera_module1 = ""

    elif i == 2:
        str_fragment_vi2 = dict_fragment_vi_2[str_rev_num]
        str_w_fragment_nvcsi2 = dict_fragment_nvcsi_ch2[str_rev_num]
        str_w_camera_module2 = dict_fragment_isx021_camera_module2[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_2_p2 = str_i2c_isx021_2_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera3 = (
                str_i2c_isx021_2_p1[str_rev_num]
                + str_w_i2c_isx021_2_p2
                + str_i2c_isx021_2_p3
                + str_i2c_isx021_2_p4
            )
            str_i2c_1_isp = ""
            str_fragment_nvcsi2 = str_w_fragment_nvcsi2
            str_camera_module2 = str_w_camera_module2
        elif camera[i] == "C2":
            str_w_i2c_imx490_2_p2 = str_i2c_imx490_2_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera3 = (
                str_i2c_imx490_2_p1[str_rev_num]
                + str_w_i2c_imx490_2_p2
                + str_i2c_imx490_2_p3
                + str_i2c_imx490_2_p4
            )
            #str_i2c_1_isp = str_i2c_isp_1
            str_fragment_nvcsi2 = str_w_fragment_nvcsi2.replace(
                "isx021_out2", "imx490_out2"
            )
            str_camera_module2 = (
                str_w_camera_module2.replace("isx021", "imx490")
                .replace("001b", "002b")
                .replace("@1b", "@2b")
            )
        else:
            str_camera3 = ""
            str_fragment_vi2 = ""
            str_fragment_nvcsi2 = ""
            str_camera_module2 = ""

    elif i == 3:
        str_fragment_vi3 = dict_fragment_vi_3[str_rev_num]
        str_w_fragment_nvcsi3 = dict_fragment_nvcsi_ch3[str_rev_num]
        str_w_camera_module3 = dict_fragment_isx021_camera_module3[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_3_p2 = str_i2c_isx021_3_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera4 = (
                str_i2c_isx021_3_p1[str_rev_num]
                + str_w_i2c_isx021_3_p2
                + str_i2c_isx021_3_p3
                + str_i2c_isx021_3_p4
            )
            str_i2c_1_isp = ""
            str_fragment_nvcsi3 = str_w_fragment_nvcsi3
            str_camera_module3 = str_w_camera_module3
        elif camera[i] == "C2":
            str_w_i2c_imx490_3_p2 = str_i2c_imx490_3_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera4 = (
                str_i2c_imx490_3_p1[str_rev_num]
                + str_w_i2c_imx490_3_p2
                + str_i2c_imx490_3_p3
                + str_i2c_imx490_3_p4
            )
            #str_i2c_1_isp = str_i2c_isp_1
            str_fragment_nvcsi3 = str_w_fragment_nvcsi3.replace(
                "isx021_out3", "imx490_out3"
            )
            str_camera_module3 = (
                str_w_camera_module3.replace("isx021", "imx490")
                .replace("001c", "002c")
                .replace("@1c", "@2c")
            )
        else:
            str_camera4 = ""
            str_fragment_vi3 = ""
            str_fragment_nvcsi3 = ""
            str_camera_module3 = ""

    elif i == 4:
        str_fragment_vi4 = dict_fragment_vi_4[str_rev_num]
        str_w_fragment_nvcsi4 = dict_fragment_nvcsi_ch4[str_rev_num]
        str_w_camera_module4 = dict_fragment_isx021_camera_module4[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_4_p2 = str_i2c_isx021_4_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera5 = (
                str_i2c_isx021_4_p1[str_rev_num]
                + str_w_i2c_isx021_4_p2
                + str_i2c_isx021_4_p3
                + str_i2c_isx021_4_p4
            )
            str_i2c_2_isp = ""
            str_fragment_nvcsi4 = str_w_fragment_nvcsi4
            str_camera_module4 = str_w_camera_module4
        elif camera[i] == "C2":
            str_w_i2c_imx490_4_p2 = str_i2c_imx490_4_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera5 = (
                str_i2c_imx490_4_p1[str_rev_num]
                + str_w_i2c_imx490_4_p2
                + str_i2c_imx490_4_p3
                + str_i2c_imx490_4_p4
            )
            #str_i2c_2_isp = str_i2c_isp_2
            str_fragment_nvcsi4 = str_w_fragment_nvcsi4.replace(
                "isx021_out4", "imx490_out4"
            )
            str_camera_module4 = (
                str_w_camera_module4.replace("isx021", "imx490")
                .replace("001b", "002b")
                .replace("@1b", "@2b")
            )
        else:
            str_camera5 = ""
            str_fragment_vi4 = ""
            str_fragment_nvcsi4 = ""
            str_camera_module4 = ""

    elif i == 5:
        str_fragment_vi5 = dict_fragment_vi_5[str_rev_num]
        str_w_fragment_nvcsi5 = dict_fragment_nvcsi_ch5[str_rev_num]
        str_w_camera_module5 = dict_fragment_isx021_camera_module5[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_5_p2 = str_i2c_isx021_5_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera6 = (
                str_i2c_isx021_5_p1[str_rev_num]
                + str_w_i2c_isx021_5_p2
                + str_i2c_isx021_5_p3
                + str_i2c_isx021_5_p4
            )
            str_i2c_2_isp = ""
            str_fragment_nvcsi5 = str_w_fragment_nvcsi5
            str_camera_module5 = str_w_camera_module5
        elif camera[i] == "C2":
            str_w_i2c_imx490_5_p2 = str_i2c_imx490_5_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera6 = (
                str_i2c_imx490_5_p1[str_rev_num]
                + str_w_i2c_imx490_5_p2
                + str_i2c_imx490_5_p3
                + str_i2c_imx490_5_p4
            )
            #str_i2c_2_isp = str_i2c_isp_2
            str_fragment_nvcsi5 = str_w_fragment_nvcsi5.replace(
                "isx021_out5", "imx490_out5"
            )
            str_camera_module5 = (
                str_w_camera_module5.replace("isx021", "imx490")
                .replace("001c", "002c")
                .replace("@1c", "@2c")
            )
        else:
            str_camera6 = ""
            str_fragment_vi5 = ""
            str_fragment_nvcsi5 = ""
            str_camera_module5 = ""

    elif i == 6:
        str_fragment_vi6 = dict_fragment_vi_6[str_rev_num]
        str_w_fragment_nvcsi6 = dict_fragment_nvcsi_ch6[str_rev_num]
        str_w_camera_module6 = dict_fragment_isx021_camera_module6[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_6_p2 = str_i2c_isx021_6_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera7 = (
                str_i2c_isx021_6_p1[str_rev_num]
                + str_w_i2c_isx021_6_p2
                + str_i2c_isx021_6_p3
                + str_i2c_isx021_6_p4
            )
            str_i2c_3_isp = ""
            str_fragment_nvcsi6 = str_w_fragment_nvcsi6
            str_camera_module6 = str_w_camera_module6
        elif camera[i] == "C2":
            str_w_i2c_imx490_6_p2 = str_i2c_imx490_6_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera7 = (
                str_i2c_imx490_6_p1[str_rev_num]
                + str_w_i2c_imx490_6_p2
                + str_i2c_imx490_6_p3
                + str_i2c_imx490_6_p4
            )
            #str_i2c_3_isp = str_i2c_isp_3
            str_fragment_nvcsi6 = str_w_fragment_nvcsi6.replace(
                "isx021_out6", "imx490_out6"
            )
            str_camera_module6 = (
                str_w_camera_module6.replace("isx021", "imx490")
                .replace("001b", "002b")
                .replace("@1b", "@2b")
            )
        else:
            str_camera7 = ""
            str_fragment_vi6 = ""
            str_fragment_nvcsi6 = ""
            str_camera_module6 = ""

    elif i == 7:
        str_fragment_vi7 = dict_fragment_vi_7[str_rev_num]
        str_w_fragment_nvcsi7 = dict_fragment_nvcsi_ch7[str_rev_num]
        str_w_camera_module7 = dict_fragment_isx021_camera_module7[str_rev_num]

        if camera[i] == "C1":
            str_w_i2c_isx021_7_p2 = str_i2c_isx021_7_p2.replace(
                dict_isx021_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera8 = (
                str_i2c_isx021_7_p1[str_rev_num]
                + str_w_i2c_isx021_7_p2
                + str_i2c_isx021_7_p3
                + str_i2c_isx021_7_p4
            )
            str_i2c_3_isp = ""
            str_fragment_nvcsi7 = str_w_fragment_nvcsi7
            str_camera_module7 = str_w_camera_module7
        elif camera[i] == "C2":
            str_w_i2c_imx490_7_p2 = str_i2c_imx490_7_p2.replace(
                dict_imx490_serdes_pix_clk["325x"],
                get_serdes_pix_clk(str_rev_num, camera[i]),
            )
            str_camera8 = (
                str_i2c_imx490_7_p1[str_rev_num]
                + str_w_i2c_imx490_7_p2
                + str_i2c_imx490_7_p3
                + str_i2c_imx490_7_p4
            )
            #str_i2c_3_isp = str_i2c_isp_3
            str_fragment_nvcsi7 = str_w_fragment_nvcsi7.replace(
                "isx021_out7", "imx490_out7"
            )
            str_camera_module7 = (
                str_w_camera_module7.replace("isx021", "imx490")
                .replace("001c", "002c")
                .replace("@1c", "@2c")
            )
        else:
            str_camera8 = ""
            str_fragment_vi7 = ""
            str_fragment_nvcsi7 = ""
            str_camera_module7 = ""

str_fragment_vi_others = dict_fragment_vi_others[str_rev_num]
str_fragment_nvcsi_others = dict_fragment_nvcsi_others[str_rev_num]
str_fragment_camera_module = dict_fragment_camera_module[str_rev_num]
str_fragment_dser_in_dtb = dict_fragment_dser_in_base_dtb[str_rev_num]

str_i2c0 = (
    str_fragment_i2c_0
    + str_i2c_dser_0[str_rev_num]
    + str_i2c_ser_0[str_rev_num]
    + str_i2c_0_isp
    + str_camera2
    + str_camera1
    + str_block_end
)

str_i2c1 = (
    str_fragment_i2c_1
    + str_i2c_dser_1[str_rev_num]
    + str_i2c_ser_1[str_rev_num]
    + str_i2c_1_isp
    + str_camera4
    + str_camera3
    + str_block_end
)

str_i2c2 = (
    str_fragment_i2c_2
    + str_i2c_dser_2[str_rev_num]
    + str_i2c_ser_2[str_rev_num]
    + str_i2c_2_isp
    + str_camera6
    + str_camera5
    + str_block_end
)

str_i2c3 = (
    str_fragment_i2c_3
    + str_i2c_dser_3[str_rev_num]
    + str_i2c_ser_3[str_rev_num]
    + str_i2c_3_isp
    + str_camera8
    + str_camera7
    + str_block_end
)

str_whole_i2c = str_i2c0 + str_i2c1 + str_i2c2 + str_i2c3


str_w_camera_type = ""
exist_c1_camera = 0
exist_c2_camera = 0

for i in range(8):
    if camera[i] == "C1":
        exist_c1_camera = 1
    elif camera[i] == "C2":
        exist_c2_camera = 1

if exist_c1_camera == 1:
    str_w_camera_type += "-isx021"
if exist_c2_camera == 1:
    str_w_camera_type += "-imx490"

str_w_overlay_header1 = dict_overlay_header[str_rev_num]

if exist_c1_camera == 0:
    str_w_overlay_header2 = str_w_overlay_header1.replace(" ISX021", "")
else:
    str_w_overlay_header2 = str_w_overlay_header1

if exist_c2_camera == 0:
    str_overlay_header = str_w_overlay_header2.replace(" IMX490", "")
else:
    str_overlay_header = str_w_overlay_header2

str_whole_dts = (
    str_overlay_header
    + str_fragment_vi0
    + str_fragment_vi1
    + str_fragment_vi2
    + str_fragment_vi3
    + str_fragment_vi4
    + str_fragment_vi5
    + str_fragment_vi6
    + str_fragment_vi7
    + str_fragment_vi_others
    + str_fragment_nvcsi0
    + str_fragment_nvcsi1
    + str_fragment_nvcsi2
    + str_fragment_nvcsi3
    + str_fragment_nvcsi4
    + str_fragment_nvcsi5
    + str_fragment_nvcsi6
    + str_fragment_nvcsi7
    + str_fragment_nvcsi_others
    + str_fragment_camera_module
    + str_camera_module0
    + str_camera_module1
    + str_camera_module2
    + str_camera_module3
    + str_camera_module4
    + str_camera_module5
    + str_camera_module6
    + str_camera_module7
    + str_fragment_i2c
    + str_i2c0
    + str_i2c1
    + str_i2c2
    + str_i2c3
    + str_fragment_dser_0[str_rev_num]
    + str_fragment_dser_1[str_rev_num]
    + str_fragment_dser_2[str_rev_num]
    + str_fragment_dser_3[str_rev_num]
    + str_fpga
    + str_overlay_end
)

overlay_dts_file_name = (
    "tier4"
    + str_w_camera_type
    + "-gmsl-device-tree-overlay-roscube-r"
    + str_rev_num
    + ".dts"
)

with open(overlay_dts_file_name, "w", encoding="utf-8") as overlay_dts_file:
    print(str_whole_dts, file=overlay_dts_file)
