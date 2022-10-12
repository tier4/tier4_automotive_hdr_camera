import sys

str_overlay_header = '''
/dts-v1/;
/plugin/;
 
/ {
    overlay-name = "Tier4 ISX021 IMX490 GMSL2 Camera Device Tree Overlay";
    compatible = "nvidia,p2822-0000+p2888-0001";
    jetson-header-name = "Jetson AGX Xavier CSI Connector";

'''

str_fragment0 = '''

    fragment@0 {

        target-path = \"/i2c@c240000/tca9546@70/i2c@0\";

        __overlay__ {

            i2c-mux,deselect-on-exit;
            #address-cells = <1>;
            #size-cells = <0>;

'''

str_ser_isp_i2c0 = '''

            ser_prim: max9295_prim@62 {
                compatible = "nvidia,tier4_max9295";
                reg = <0x62>;
                is-prim-ser;
            };
            ser_a: max9295_a@42 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x42>;
                nvidia,gmsl-dser-device = <&dser>;
            };
            ser_b: max9295_b@60 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x60>;
                nvidia,gmsl-dser-device = <&dser>;
            };
            isp_prim: gw5300_prim@6d {
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
'''

str_imx490_b_2c_i2c0 = '''

            imx490_b@2c {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
              //  clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_b>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_b>;
                nvidia,gmsl-dser-device = <&dser>;
                reg = <0x2c>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <0>;
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
            
                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = "\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
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
                
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        imx490_out1: endpoint {
                            vc-id = <1>;
                            port-index = <0>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in1>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            }; 
'''

str_imx490_a_2b_i2c0 = '''

            imx490_a@2b {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
            //    clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_a>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_a>;
                nvidia,gmsl-dser-device = <&dser>;
                reg = <0x2b>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <0>;
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

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *   
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
                
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
                };
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
            };
'''

str_isx021_b_1c_i2c0 = '''

            isx021_b@1c {
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,gmsl-ser-device = <&ser_b>;
                nvidia,gmsl-dser-device = <&dser>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1c>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <0>;
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
                use_sensor_mode_id = \"true\";

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_a\";
                    vc_id = \"1\";
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
                    min_gain_val = \"0\";                           /* dB */
                    max_gain_val = \"300\";                             /* dB */
                    step_gain_val = \"3\";                          /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\";                          /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";                       /* us */
                    embedded_metadata_height = \"0\";
                };

                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out1: endpoint {
                            vc-id = <1>;
                            port-index = <0>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in1>;
                        };
                    };
                };

                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_isx021_a_1b_i2c0 = '''

            isx021_a@1b {

                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,gmsl-ser-device  = <&ser_a>;
                nvidia,gmsl-dser-device = <&dser>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1b>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <0>;
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
                use_sensor_mode_id = \"true\";

                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
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
                    min_gain_val = \"0\";                       /* dB */
                    max_gain_val = \"300\";                         /* dB */
                    step_gain_val = \"3\";                      /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\";                      /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";                   /* us */
                    embedded_metadata_height = \"0\";
                };
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
                gmsl-link {
                    src-csi-port = \"b\";       /* Port at which sensor is connected to its serializer device. */
                    dst-csi-port = \"a\";       /* Destination CSI port on the Jetson side, connected at deserializer. */
                    serdes-csi-link = \"a\";    /* GMSL link sensor/serializer connected */
                    csi-mode = \"1x4\";     /*  to sensor CSI mode. */
                    st-vc = <0>;            /* Sensor source default VC ID: 0 unless overridden by sensor. */
                    vc-id = <0>;            /* Destination VC ID, assigned to sensor stream by deserializer. */
                    num-lanes = <2>;        /* Number of CSI lanes used. */
                    streams = \"ued-u1\",\"yuv8\"; /* Types of streams sensor is streaming. */
                };
            };
'''

str_fragment1 = '''

    fragment@1 {

        target-path = \"/i2c@c240000/tca9546@70/i2c@1\";

        __overlay__ {

            i2c-mux,deselect-on-exit;
            #address-cells = <1>;
            #size-cells = <0>;
'''

str_ser_isp_i2c1 = '''

            ser_prima: max9295_prim@62 {
                compatible = "nvidia,tier4_max9295\";
                reg = <0x62>;
                is-prim-ser;
            };
            ser_c: max9295_a@42 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x42>;
                nvidia,gmsl-dser-device = <&dsera>;
            };
            ser_d: max9295_b@60 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x60>;
                nvidia,gmsl-dser-device = <&dsera>;
            };

            isp_prima: gw5300_prim@6d {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6d>;
                is-prim-isp;
            };
            isp_c: gw5300_a@6e {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6e>;
            };
            isp_d: gw5300_b@6f {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6f>;
            };
'''

str_imx490_d_2c_i2c1 = '''

            imx490_d@2c {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
                //clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                //clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_d>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_d>;
                nvidia,gmsl-dser-device = <&dsera>;
                reg = <0x2c>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <1>;
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
                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
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
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        imx490_out3: endpoint {
                            vc-id = <1>;
                            port-index = <2>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in3>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_imx490_c_2b_i2c1 = '''

            imx490_c@2b {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
                // clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                //clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_c>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_c>;
                nvidia,gmsl-dser-device = <&dsera>;
                reg = <0x2b>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <1>;
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
                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
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
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        imx490_out2: endpoint {
                            vc-id = <0>;
                            port-index = <2>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in2>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";             /* Port at which sensor  is connected to its serializer device. */
                    dst-csi-port = \"a\";             /* Destination CSI port on the Jetson side, connected at deserializer. */
                    serdes-csi-link = \"a\";          /* GMSL link sensor/serializer connected */
                    csi-mode = \"1x4\";               /*  to sensor CSI mode. */
                    st-vc = <0>;                    /* Sensor source default VC ID: 0 unless overridden by sensor. */
                    vc-id = <0>;                    /* Destination VC ID, assigned to sensor stream by deserializer. */
                    num-lanes = <2>;                /* Number of CSI lanes used. */
                    streams = \"ued-u1\",\"yuv8\";      /* Types of streams sensor is streaming. */
                };
            };
'''

str_isx021_d_1c_i2c1 = ''' 

            isx021_d@1c {

                def-addr = <0x1a>;
            /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = "extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
    
            nvidia,gmsl-ser-device = <&ser_d>;
                nvidia,gmsl-dser-device = <&dsera>;
    
            compatible = \"nvidia,tier4_isx021\";
    
            reg = <0x1c>;
    
            /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <1>;
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
                use_sensor_mode_id = \"true\";
    
            /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_c\";
                    vc_id = \"1\";
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
                    min_gain_val = \"0\"; /* dB */
                    max_gain_val = \"300\"; /* dB */
                    step_gain_val = \"3\"; /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\"; /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";/* us */
                    embedded_metadata_height = \"0\";
                };
    
            ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out3: endpoint {
                            vc-id = <1>;
                            port-index = <2>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in3>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_isx021_c_1b_i2c1 = ''' 

            isx021_c@1b {

                def-addr = <0x1a>;

                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";

                nvidia,gmsl-ser-device = <&ser_c>;
                nvidia,gmsl-dser-device = <&dsera>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1b>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <1>;
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
                use_sensor_mode_id = \"true\";

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_c\";
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
                    min_gain_val = \"0\"; /* dB */
                    max_gain_val = \"300\"; /* dB */
                    step_gain_val = \"3\"; /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\"; /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";/* us */
                    embedded_metadata_height = \"0\";
                };

                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out2: endpoint {
                            vc-id = <0>;
                            port-index = <2>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in2>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";               /* Port at which sensor  is connected to its serializer device. */
                    dst-csi-port = \"a\";               /* Destination CSI port on the Jetson side, connected at deserializer. */
                    serdes-csi-link = \"a\";            /* GMSL link sensor/serializer connected */
                    csi-mode = \"1x4\";             /*  to sensor CSI mode. */
                    st-vc = <0>;                    /* Sensor source default VC ID: 0 unless overridden by sensor. */
                    vc-id = <0>;                    /* Destination VC ID, assigned to sensor stream by deserializer. */
                    num-lanes = <2>;                /* Number of CSI lanes used. */
                    streams = \"ued-u1\",\"yuv8\";      /* Types of streams sensor is streaming. */
                };
            };
'''

str_fragment2 = '''

    fragment@2 {

        target-path = \"/i2c@c240000/tca9546@70/i2c@2\";

        __overlay__ {

            i2c-mux,deselect-on-exit;
            #address-cells = <1>;
            #size-cells = <0>;
'''

str_ser_isp_i2c2 = '''

            ser_primb: max9295_prim@62 {
                compatible = "nvidia,tier4_max9295\";
                reg = <0x62>;
                is-prim-ser;
            };
            ser_e: max9295_a@42 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x42>;
                nvidia,gmsl-dser-device = <&dserb>;
            };
            ser_f: max9295_b@60 {
            compatible = \"nvidia,tier4_max9295\";
                reg = <0x60>;
                nvidia,gmsl-dser-device = <&dserb>;
            };

            isp_primb: gw5300_prim@6d {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6d>;
                is-prim-isp;
            };
            isp_e: gw5300_a@6e {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6e>;
            };
            isp_f: gw5300_b@6f {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6f>;
            };
'''

str_imx490_f_2c_i2c2 = '''

            imx490_f@2c {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
                //clocks = <&bpmp_clks 36 >, <&bpmp_clks 36 >;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_f>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_f>;
                nvidia,gmsl-dser-device = <&dserb>;
                reg = <0x2c>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <2>;
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
                 /**
                 * A modeX node is required to support v4l2 driver
                 * implementation with NVIDIA camera software stack
                 *
                 * mclk_khz = \"\";
                 * Standard MIPI driving clock, typically 24MHz
                 *
                 * num_lanes = \"\";
                 * Number of lane channels sensor is programmed to output
                 *
                 * tegra_sinterface = \"\";
                 * The base tegra serial interface lanes are connected to
                 *
                 * vc_id = \"\";
                 * The virtual channel id of the sensor.
                 *
                 * discontinuous_clk = \"\";
                 * The sensor is programmed to use a discontinuous clock on MIPI lanes
                 *
                 * dpcm_enable = \"true\";
                 * The sensor is programmed to use a DPCM modes
                 *
                 * cil_settletime = \"\";
                 * MIPI lane settle time value.
                 * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                 *
                 * active_w = \"\";
                 * Pixel active region width
                 *
                 * active_h = \"\";
                 * Pixel active region height
                 *
                 * dynamic_pixel_bit_depth = \"\";
                 * sensor dynamic bit depth for sensor mode
                 *
                 * csi_pixel_bit_depth = \"\";
                 * sensor output bit depth for sensor mode
                 *
                 * mode_type=\"\";
                 * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                 *
                 * pixel_phase=\"\";
                 * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                 *
             * readout_orientation = \"0\";
                 * Based on camera module orientation.
                 * Only change readout_orientation if you specifically
                 * Program a different readout order for this mode
                 *
                 * line_length = \"\";
                 * Pixel line length (width) for sensor mode.
                 * This is used to calibrate features in our camera stack.
                 *
                 * pix_clk_hz = \"\";
                 * Sensor pixel clock used for calculations like exposure and framerate
                 *
                 *
                 *
                 *
                 * inherent_gain = \"\";
                 * Gain obtained inherently from mode (ie. pixel binning)
                 *
                 * min_gain_val = \"\"; (floor to 6 decimal places)
                 * max_gain_val = \"\"; (floor to 6 decimal places)
             * Gain limits for mode
                 * if use_decibel_gain = \"true\", please set the gain as decibel
                 *
                 * min_exp_time = \"\"; (ceil to integer)
                 * max_exp_time = \"\"; (ceil to integer)
                 * Exposure Time limits for mode (us)
                 *
                 *
                 * min_hdr_ratio = \"\";
                 * max_hdr_ratio = \"\";
                 * HDR Ratio limits for mode
                 *
                 * min_framerate = \"\";
                 * max_framerate = \"\";
                 * Framerate limits for mode (fps)
                 *
                 * embedded_metadata_height = \"\";
                 * Sensor embedded metadata height in units of rows.
                 * If sensor does not support embedded metadata value should be 0.
                 */
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
                 ports {
                     #address-cells = <1>;
                 #size-cells = <0>;
                     port@0 {
                         reg = <0>;
                         imx490_out5: endpoint {
                             vc-id = <1>;
                             port-index = <4>;
                             bus-width = <2>;
                             remote-endpoint = <&csi_in5>;
                         };
                     };
                 };
                 gmsl-link {
                     src-csi-port = \"b\";
                     dst-csi-port = \"a\";
                     serdes-csi-link = \"b\";
                     csi-mode = \"1x4\";
                     st-vc = <0>;
                     vc-id = <1>;
                     num-lanes = <2>;
                     streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_imx490_e_2b_i2c2 = '''

            imx490_e@2b {
                 compatible = \"nvidia,tier4_imx490\";
                 def-addr = <0x1a>;
                 /* Define any required hw resources needed by driver */
                 /* ie. clocks, io pins, power sources */
               //  clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                 clock-names = \"extperiph1\", \"pllp_grtba\";
                 mclk = \"extperiph1\";
                 nvidia,isp-device = <&isp_e>;          // for C2 camera
                 nvidia,gmsl-ser-device = <&ser_e>;
                 nvidia,gmsl-dser-device = <&dserb>;
                 reg = <0x2b>;
                 /* Physical dimensions of sensor */
                 physical_w = \"15.0\";
                 physical_h = \"12.5\";
                 reg_mux = <2>;
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
                 /**
                 * A modeX node is required to support v4l2 driver
                 * implementation with NVIDIA camera software stack
                 *
                 * mclk_khz = \"\";
                 * Standard MIPI driving clock, typically 24MHz
                 *
                 * num_lanes = \"\";
                 * Number of lane channels sensor is programmed to output
                 *
                 * tegra_sinterface = \"\";
                 * The base tegra serial interface lanes are connected to
                 *
                 * vc_id = \"\";
                 * The virtual channel id of the sensor.
                 *
                 * discontinuous_clk = \"\";
                 * The sensor is programmed to use a discontinuous clock on MIPI lanes
                 *
                 * dpcm_enable = \"true\";
                 * The sensor is programmed to use a DPCM modes
                 *
                 * cil_settletime = \"\";
                 * MIPI lane settle time value.
                 * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                 *
                 * active_w = \"\";
                 * Pixel active region width
                 *
                 * active_h = \"\";
                 * Pixel active region height
                 *
                 * dynamic_pixel_bit_depth = \"\";
                 * sensor dynamic bit depth for sensor mode
                 *
                 * csi_pixel_bit_depth = \"\";
                 * sensor output bit depth for sensor mode
                 *
                 * mode_type=\"\";
                 * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                 *
                 * pixel_phase=\"\";
                 * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                 *
                 * readout_orientation = \"0\";
                 * Based on camera module orientation.
                 * Only change readout_orientation if you specifically
                 * Program a different readout order for this mode
                 *
                 * line_length = \"\";
                 * Pixel line length (width) for sensor mode.
                 * This is used to calibrate features in our camera stack.
                 *
                 * pix_clk_hz = \"\";
                 * Sensor pixel clock used for calculations like exposure and framerate
                 *
                 *
                 *
                 *
                 * inherent_gain = \"\";
                 * Gain obtained inherently from mode (ie. pixel binning)
                 *
                 * min_gain_val = \"\"; (floor to 6 decimal places)
                 * max_gain_val = \"\"; (floor to 6 decimal places)
                 * Gain limits for mode
                 * if use_decibel_gain = \"true\", please set the gain as decibel
                 *
                 * min_exp_time = \"\"; (ceil to integer)
                 * max_exp_time = \"\"; (ceil to integer)
                 * Exposure Time limits for mode (us)
                 *
                 *
                 * min_hdr_ratio = \"\";
                 * max_hdr_ratio = \"\";
                 * HDR Ratio limits for mode
                 *
                 * min_framerate = \"\";
                 * max_framerate = \"\";
                 * Framerate limits for mode (fps)
                 *
                 * embedded_metadata_height = \"\";
                 * Sensor embedded metadata height in units of rows.
                 * If sensor does not support embedded metadata value should be 0.
                 */
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
                 ports {
                     #address-cells = <1>;
                     #size-cells = <0>;
                     port@0 {
                         reg = <0>;
                         imx490_out4: endpoint {
                             vc-id = <0>;
                             port-index = <4>;
                         bus-width = <2>;
                         remote-endpoint = <&csi_in4>;
                     };
                 };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                 serdes-csi-link = \"a\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <0>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_isx021_f_1c_i2c2 = '''

            isx021_f@1c {

                def-addr = <0x1a>;

                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = "extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";

                nvidia,gmsl-ser-device = <&ser_f>;
                nvidia,gmsl-dser-device = <&dserb>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1c>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <2>;
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
                use_sensor_mode_id = \"true\";

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
    
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_e\";
                    vc_id = \"1\";
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
                    min_gain_val = \"0\"; /* dB */
                    max_gain_val = \"300\"; /* dB */
                    step_gain_val = \"3\"; /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\"; /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";/* us */
                    embedded_metadata_height = \"0\";
            };
    
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out5: endpoint {
                            vc-id = <1>;
                            port-index = <4>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in5>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
        };
'''

str_isx021_e_1b_i2c2 = '''

        isx021_e@1b {

            def-addr = <0x1a>;

            /* Define any required hw resources needed by driver */
            /* ie. clocks, io pins, power sources */
        //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
        //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
            clock-names = \"extperiph1\", \"pllp_grtba\";
            mclk = \"extperiph1\";

            nvidia,gmsl-ser-device = <&ser_e>;
            nvidia,gmsl-dser-device = <&dserb>;

            compatible = \"nvidia,tier4_isx021\";

            reg = <0x1b>;

            /* Physical dimensions of sensor */
            physical_w = \"15.0\";
            physical_h = \"12.5\";
            reg_mux = <2>;
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
            use_sensor_mode_id = \"true\";

            /**
            * A modeX node is required to support v4l2 driver
            * implementation with NVIDIA camera software stack
            *
            * mclk_khz = \"\";
            * Standard MIPI driving clock, typically 24MHz
            *
            * num_lanes = \"\";
            * Number of lane channels sensor is programmed to output
            *
            * tegra_sinterface = \"\";
            * The base tegra serial interface lanes are connected to
            *
            * vc_id = \"\";
            * The virtual channel id of the sensor.
            *
            * discontinuous_clk = \"\";
            * The sensor is programmed to use a discontinuous clock on MIPI lanes
            *
            * dpcm_enable = \"true\";
            * The sensor is programmed to use a DPCM modes
            *
            * cil_settletime = \"\";
            * MIPI lane settle time value.
            * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
            *
            * active_w = \"\";
            * Pixel active region width
            *
            * active_h = \"\";
            * Pixel active region height
            *
            * dynamic_pixel_bit_depth = \"\";
            * sensor dynamic bit depth for sensor mode
            *
            * csi_pixel_bit_depth = \"\";
            * sensor output bit depth for sensor mode
            *
            * mode_type=\"\";
            * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
            *
            * pixel_phase=\"\";
            * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
            *
            * readout_orientation = \"0\";
            * Based on camera module orientation.
            * Only change readout_orientation if you specifically
            * Program a different readout order for this mode
            *
            * line_length = \"\";
            * Pixel line length (width) for sensor mode.
            * This is used to calibrate features in our camera stack.
            *
            * pix_clk_hz = \"\";
            * Sensor pixel clock used for calculations like exposure and framerate
            *
            *
            *
            *
            * inherent_gain = \"\";
            * Gain obtained inherently from mode (ie. pixel binning)
            *
            * min_gain_val = \"\"; (floor to 6 decimal places)
            * max_gain_val = \"\"; (floor to 6 decimal places)
            * Gain limits for mode
            * if use_decibel_gain = \"true\", please set the gain as decibel
            *
            * min_exp_time = \"\"; (ceil to integer)
            * max_exp_time = \"\"; (ceil to integer)
            * Exposure Time limits for mode (us)
            *
            *
            * min_hdr_ratio = \"\";
            * max_hdr_ratio = \"\";
            * HDR Ratio limits for mode
            *
            * min_framerate = \"\";
            * max_framerate = \"\";
            * Framerate limits for mode (fps)
            *
            * embedded_metadata_height = \"\";
            * Sensor embedded metadata height in units of rows.
            * If sensor does not support embedded metadata value should be 0.
            */

            mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                mclk_khz = \"24000\";
                num_lanes = \"2\";
                tegra_sinterface = \"serial_e\";
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
                min_gain_val = \"0\"; /* dB */
                max_gain_val = \"300\"; /* dB */
                step_gain_val = \"3\"; /* 0.3 */
                default_gain = \"0\";
                min_hdr_ratio = \"1\";
                max_hdr_ratio = \"1\";
                framerate_factor = \"1000000\";
                min_framerate = \"30000000\";
                max_framerate = \"30000000\";
                step_framerate = \"1\";
                default_framerate = \"30000000\";
                exposure_factor = \"1000000\";
                min_exp_time = \"24\"; /* us 1 line */
                max_exp_time = \"33333\";
                step_exp_time = \"1\";
                default_exp_time = \"33333\";/* us */
                embedded_metadata_height = \"0\";
            };

            ports {
                #address-cells = <1>;
                #size-cells = <0>;
                port@0 {
                    reg = <0>;
                    isx021_out4: endpoint {
                        vc-id = <0>;
                        port-index = <4>;
                        bus-width = <2>;
                        remote-endpoint = <&csi_in4>;
                        };
                    };
                };
            gmsl-link {
                src-csi-port = \"b\";
                dst-csi-port = \"a\";
                serdes-csi-link = \"a\";
                csi-mode = \"1x4\";
                st-vc = <0>;
                vc-id = <0>;
                num-lanes = <2>;
                streams = \"ued-u1\",\"yuv8\";
            };
        };
'''

str_fragment3 = '''

    fragment@3 {
        target-path = \"/i2c@c240000/tca9546@70/i2c@3\";
        __overlay__ {
            i2c-mux,deselect-on-exit;
            #address-cells = <1>;
            #size-cells = <0>;
'''

str_ser_isp_i2c3 = '''

            ser_primc: max9295_prim@62 {
                compatible = "nvidia,tier4_max9295\";
                reg = <0x62>;
                is-prim-ser;
            };
            ser_g: max9295_a@42 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x42>;
                nvidia,gmsl-dser-device = <&dserc>;
            };
            ser_h: max9295_b@60 {
                compatible = \"nvidia,tier4_max9295\";
                reg = <0x60>;
                nvidia,gmsl-dser-device = <&dserc>;
            };
            isp_primc: gw5300_prim@6d {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6d>;
                is-prim-isp;
            };
            isp_g: gw5300_a@6e {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6e>;
            };
            isp_h: gw5300_b@6f {
                compatible = \"nvidia,tier4_gw5300\";
                reg = <0x6f>;
            };
'''

str_imx490_h_2c_i2c3 = '''

            imx490_h@2c {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
                //clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_h>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_h>;
                nvidia,gmsl-dser-device = <&dserc>;
                reg = <0x2c>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <3>;
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
                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
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
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        imx490_out7: endpoint {
                            vc-id = <1>;
                            port-index = <5>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in7>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_imx490_g_2b_i2c3 = '''

            imx490_g@2b {
                compatible = \"nvidia,tier4_imx490\";
                def-addr = <0x1a>;
                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
               // clocks = <&bpmp_clks 36>, <&bpmp_clks 36>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";
                nvidia,isp-device = <&isp_g>;           // for C2 camera
                nvidia,gmsl-ser-device = <&ser_g>;
                nvidia,gmsl-dser-device = <&dserc>;
                reg = <0x2b>;
                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <3>;
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
                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
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
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        imx490_out6: endpoint {
                            vc-id = <0>;
                            port-index = <5>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in6>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"a\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <0>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_isx021_h_1c_i2c3 = '''

            isx021_h@1c {

                def-addr = <0x1a>;

                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";

                nvidia,gmsl-ser-device = <&ser_h>;
                nvidia,gmsl-dser-device = <&dserc>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1c>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <3>;
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
                use_sensor_mode_id = \"true\";

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
    
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_g\";
                    vc_id = \"1\";
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
                    min_gain_val = \"0\"; /* dB */
                    max_gain_val = \"300\"; /* dB */
                    step_gain_val = \"3\"; /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\"; /*us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";/* us */
                    embedded_metadata_height = \"0\";
                };
    
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out7: endpoint {
                            vc-id = <1>;
                            port-index = <5>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in7>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"b\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <1>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_isx021_g_1b_i2c3 = '''

            isx021_g@1b {

                def-addr = <0x1a>;

                /* Define any required hw resources needed by driver */
                /* ie. clocks, io pins, power sources */
            //  clocks = <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>,
            //          <&bpmp_clks TEGRA194_CLK_EXTPERIPH1>;
                clock-names = \"extperiph1\", \"pllp_grtba\";
                mclk = \"extperiph1\";

                nvidia,gmsl-ser-device = <&ser_g>;
                nvidia,gmsl-dser-device = <&dserc>;

                compatible = \"nvidia,tier4_isx021\";

                reg = <0x1b>;

                /* Physical dimensions of sensor */
                physical_w = \"15.0\";
                physical_h = \"12.5\";
                reg_mux = <3>;
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
                use_sensor_mode_id = \"true\";

                /**
                * A modeX node is required to support v4l2 driver
                * implementation with NVIDIA camera software stack
                *
                * mclk_khz = \"\";
                * Standard MIPI driving clock, typically 24MHz
                *
                * num_lanes = \"\";
                * Number of lane channels sensor is programmed to output
                *
                * tegra_sinterface = \"\";
                * The base tegra serial interface lanes are connected to
                *
                * vc_id = \"\";
                * The virtual channel id of the sensor.
                *
                * discontinuous_clk = \"\";
                * The sensor is programmed to use a discontinuous clock on MIPI lanes
                *
                * dpcm_enable = \"true\";
                * The sensor is programmed to use a DPCM modes
                *
                * cil_settletime = \"\";
                * MIPI lane settle time value.
                * A \"0\" value attempts to autocalibrate based on mclk_khz and pix_clk_hz
                *
                * active_w = \"\";
                * Pixel active region width
                *
                * active_h = \"\";
                * Pixel active region height
                *
                * dynamic_pixel_bit_depth = \"\";
                * sensor dynamic bit depth for sensor mode
                *
                * csi_pixel_bit_depth = \"\";
                * sensor output bit depth for sensor mode
                *
                * mode_type=\"\";
                * Sensor mode type, For eg: yuv, Rgb, bayer, bayer_wdr_pwl
                *
                * pixel_phase=\"\";
                * Pixel phase for sensor mode, For eg: rggb, vyuy, rgb888
                *
                * readout_orientation = \"0\";
                * Based on camera module orientation.
                * Only change readout_orientation if you specifically
                * Program a different readout order for this mode
                *
                * line_length = \"\";
                * Pixel line length (width) for sensor mode.
                * This is used to calibrate features in our camera stack.
                *
                * pix_clk_hz = \"\";
                * Sensor pixel clock used for calculations like exposure and framerate
                *
                *
                *
                *
                * inherent_gain = \"\";
                * Gain obtained inherently from mode (ie. pixel binning)
                *
                * min_gain_val = \"\"; (floor to 6 decimal places)
                * max_gain_val = \"\"; (floor to 6 decimal places)
                * Gain limits for mode
                * if use_decibel_gain = \"true\", please set the gain as decibel
                *
                * min_exp_time = \"\"; (ceil to integer)
                * max_exp_time = \"\"; (ceil to integer)
                * Exposure Time limits for mode (us)
                *
                *
                * min_hdr_ratio = \"\";
                * max_hdr_ratio = \"\";
                * HDR Ratio limits for mode
                *
                * min_framerate = \"\";
                * max_framerate = \"\";
                * Framerate limits for mode (fps)
                *
                * embedded_metadata_height = \"\";
                * Sensor embedded metadata height in units of rows.
                * If sensor does not support embedded metadata value should be 0.
                */
    
                mode0 {/*mode ISX021_MODE_1920X1280_CROP_30FPS*/
                    mclk_khz = \"24000\";
                    num_lanes = \"2\";
                    tegra_sinterface = \"serial_g\";
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
                    min_gain_val = \"0\"; /* dB */
                    max_gain_val = \"300\"; /* dB */
                    step_gain_val = \"3\"; /* 0.3 */
                    default_gain = \"0\";
                    min_hdr_ratio = \"1\";
                    max_hdr_ratio = \"1\";
                    framerate_factor = \"1000000\";
                    min_framerate = \"30000000\";
                    max_framerate = \"30000000\";
                    step_framerate = \"1\";
                    default_framerate = \"30000000\";
                    exposure_factor = \"1000000\";
                    min_exp_time = \"24\"; /* us 1 line */
                    max_exp_time = \"33333\";
                    step_exp_time = \"1\";
                    default_exp_time = \"33333\";/* us */
                    embedded_metadata_height = \"0\";
            };
    
                ports {
                    #address-cells = <1>;
                    #size-cells = <0>;
                    port@0 {
                        reg = <0>;
                        isx021_out6: endpoint {
                            vc-id = <0>;
                            port-index = <5>;
                            bus-width = <2>;
                            remote-endpoint = <&csi_in6>;
                        };
                    };
                };
                gmsl-link {
                    src-csi-port = \"b\";
                    dst-csi-port = \"a\";
                    serdes-csi-link = \"a\";
                    csi-mode = \"1x4\";
                    st-vc = <0>;
                    vc-id = <0>;
                    num-lanes = <2>;
                    streams = \"ued-u1\",\"yuv8\";
                };
            };
'''

str_fragment_vi_0 = '''

    fragment@4 {
      target = <&vi_port0>;
      __overlay__ {
        reg = <0>;
        status = \"okay\";
      };
    };

    fragment@5 {
      target = <&vi_in0>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&csi_out0>;
      };
    };
'''

str_fragment_vi_1 = '''

    fragment@6 {
      target = <&vi_port1>;
      __overlay__ {
        reg = <1>;
        status = \"okay\";
      };
    };

    fragment@7 {
      target = <&vi_in1>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <0>;
        bus-width = <2>;
        remote-endpoint = <&csi_out1>;
      };
    };
'''

str_fragment_vi_2 = '''

    fragment@8 {
      target = <&vi_port2>;
      __overlay__ {
        reg = <2>;
        status = \"okay\";
      };
    };

    fragment@9 {
      target = <&vi_in2>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&csi_out2>;
      };
    };
'''

str_fragment_vi_3 = '''

    fragment@10 {
      target = <&vi_port3>;
      __overlay__ {
        reg = <3>;
        status = \"okay\";
      };
    };

    fragment@11 {
      target = <&vi_in3>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <2>;
        bus-width = <2>;
        remote-endpoint = <&csi_out3>;
      };
    };
'''

str_fragment_vi_4 = '''

    fragment@12 {
      target = <&vi_port4>;
      __overlay__ {
        reg = <4>;
        status = \"okay\";
      };
    };

    fragment@13 {
      target = <&vi_in4>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&csi_out4>;
      };
    };
'''

str_fragment_vi_5 = '''

    fragment@14 {
      target = <&vi_port5>;
      __overlay__ {
        reg = <5>;
        status = \"okay\";
      };
    };

    fragment@15 {
      target = <&vi_in5>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <4>;
        bus-width = <2>;
        remote-endpoint = <&csi_out5>;
      };
    };
'''

str_fragment_vi_6 = '''

    fragment@16 {
      target = <&vi_port6>;
      __overlay__ {
        reg = <6>;
        status = \"okay\";
      };
    };

    fragment@17 {
      target = <&vi_in6>;
      __overlay__ {
        status = \"okay\";
        vc-id = <0>;
        port-index = <5>;
        bus-width = <2>;
        remote-endpoint = <&csi_out6>;
      };
    };
'''

str_fragment_vi_7 = '''

    fragment@18 {
      target = <&vi_port7>;
      __overlay__ {
        reg = <7>;
        status = \"okay\";
      };
    };

    fragment@19 {
      target = <&vi_in7>;
      __overlay__ {
        status = \"okay\";
        vc-id = <1>;
        port-index = <5>;
        bus-width = <2>;
        remote-endpoint = <&csi_out7>;
      };
    };
'''

str_fragment_nvcsi_ch0 = '''

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
        remote-endpoint = <&imx490_out0>;
      };
    };

    fragment@24 {
      target = <&csi_out0>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in0>;
      };
    };
'''

str_fragment_nvcsi_ch1 = '''

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
        remote-endpoint = <&imx490_out1>;
      };
    };

    fragment@29 {
      target = <&csi_out1>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in1>;
      };
    };
'''

str_fragment_nvcsi_ch2 = '''

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
        remote-endpoint = <&imx490_out2>;
      };
    };

    fragment@34 {
      target = <&csi_out2>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in2>;
      };
    };
'''

str_fragment_nvcsi_ch3 = '''

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
        remote-endpoint = <&imx490_out3>;
      };
    };

    fragment@39 {
      target = <&csi_out3>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in3>;
      };
    };
'''

str_fragment_nvcsi_ch4 = '''

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
        remote-endpoint = <&imx490_out4>;
      };
    };

    fragment@44 {
      target = <&csi_out4>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in4>;
      };
    };
'''

str_fragment_nvcsi_ch5 = '''

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
        remote-endpoint = <&imx490_out5>;
      };
    };

    fragment@49 {
      target = <&csi_out5>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in5>;
      };
    };
'''

str_fragment_nvcsi_ch6 = '''

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
        remote-endpoint = <&imx490_out6>;
      };
    };

    fragment@54 {
      target = <&csi_out6>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in6>;
      };
    };
'''

str_fragment_nvcsi_ch7 = '''

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
        remote-endpoint = <&imx490_out7>;
      };
    };

    fragment@59 {
      target = <&csi_out7>;
      __overlay__ {
        status = \"okay\";
        remote-endpoint = <&vi_in7>;
      };
    };
'''

str_imx490_cameramodule0 = '''

    /* camera 0 */
    fragment@60 {
      target = <&camera_module0>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_rear_liimx490\";
        position = \"rear\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 30-002b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/imx490_a@2b\";
        };
      };
    }; 
'''

str_imx490_cameramodule1 = '''

    fragment@61 {
      target = <&camera_module1>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_front_liimx490\";
        position = \"front\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 30-002c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/imx490_b@2c\";
        };
      };
    }; 
'''

str_imx490_cameramodule2 = '''

    fragment@62 {
      target = <&camera_module2>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_topright_liimx490\";
        position = \"topright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 31-002b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/imx490_c@2b\";
        };
      };
    }; 
'''

str_imx490_cameramodule3 = '''

    fragment@63 {
      target = <&camera_module3>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_bottomright_liimx490\";
        position = \"bottomright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 31-002c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/imx490_d@2c\";
        };
      };
    }; 
'''

str_imx490_cameramodule4 = '''

    fragment@64 {
      target = <&camera_module4>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_topleft_liimx490\";
        position = \"topleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 32-002b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/imx490_e@2b\";
        };
      };
    }; 
'''

str_imx490_cameramodule5 = '''

    fragment@65 {
      target = <&camera_module5>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_centerright_liimx490\";
        position = \"centerright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 32-002c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/imx490_f@2c\";
        };
      };
    }; 
'''

str_imx490_cameramodule6 = '''

    fragment@66 {
      target = <&camera_module6>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_centerleft_liimx490\";
        position = \"centerleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 33-002b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/imx490_g@2b\";
        };
      };
    }; 
'''

str_imx490_cameramodule7 = '''

    fragment@67 {
      target = <&camera_module7>;
      __overlay__ {
        status = \"okay\";
        badge = \"imx490_bottomleft_liimx490\";
        position = \"bottomleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"imx490 33-002c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/imx490_h@2c\";
        };
      };
    }; 
'''

str_isx021_cameramodule0 = '''

    /* camera 0 */
    fragment@60 {
      target = <&camera_module0>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_rear_liisx021\";
        position = \"rear\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 30-001b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_a@1b\";
        };
      };
    }; 
'''

str_isx021_cameramodule1 = '''

    fragment@61 {
      target = <&camera_module1>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_front_liisx021\";
        position = \"front\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 30-001c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@0/isx021_b@1c\";
        };
      };
    }; 
'''

str_isx021_cameramodule2 = '''

    fragment@62 {
      target = <&camera_module2>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_topright_liisx021\";
        position = \"topright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 31-001b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_c@1b\";
        };
      };
    }; 
'''

str_isx021_cameramodule3 = '''

    fragment@63 {
      target = <&camera_module3>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_bottomright_liisx021\";
        position = \"bottomright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 31-001c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@1/isx021_d@1c\";
        };
      };
    }; 
'''

str_isx021_cameramodule4 = '''

    fragment@64 {
      target = <&camera_module4>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_topleft_liisx021\";
        position = \"topleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 32-001b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_e@1b\";
        };
      };
    }; 
'''

str_isx021_cameramodule5 = '''

    fragment65 {
      target = <&camera_module5>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_centerright_liisx021\";
        position = \"centerright\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 32-001c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@2/isx021_f@1c\";
        };
      };
    }; 
'''

str_isx021_cameramodule6 = '''

    fragment@66 {
      target = <&camera_module6>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_centerleft_liisx021\";
        position = \"centerleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 33-001b\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/isx021_g@1b\";
        };
      };
    }; 
'''

str_isx021_cameramodule7 = '''

    fragment@67 {
      target = <&camera_module7>;
      __overlay__ {
        status = \"okay\";
        badge = \"isx021_bottomleft_liisx021\";
        position = \"bottomleft\";
        orientation = \"1\";
        drivernode0 {
          status = \"okay\";
          pcl_id = \"v4l2_sensor\";
          devname = \"isx021 33-001c\";
          proc-device-tree = \"/proc/device-tree/i2c@c240000/tca9546@70/i2c@3/isx021_h@1c\";
        };
      };
    }; 
'''

str_dser_fragment = '''

    fragment@68 {
      target = <&dser>;
      __overlay__ {
        compatible = \"nvidia,tier4_max9296\";
      };
    }; 

    fragment@69 {
      target = <&dsera>;
      __overlay__ {
        compatible = \"nvidia,tier4_max9296\";
      };
    }; 

    fragment@70 {
      target = <&dserb>;
      __overlay__ {
        compatible = \"nvidia,tier4_max9296\";
      };
    }; 

    fragment@71 {
      target = <&dserc>;
      __overlay__ {
        compatible = \"nvidia,tier4_max9296\";
      };
    }; 

    fragment@72 {
      target-path = \"/host1x/vi@15c10000\";
      __overlay__ {
        num-channels = <8>;
      };
    };
'''

str_block_end = '''
        };
    };
'''

str_overlay_end = '''

};
'''

def idenify_camera(cmd_arg):
    upper_arg = cmd_arg.upper()
    if  upper_arg == 'C1' :
        return 'C1'
    elif upper_arg == 'C2' :
        return 'C2'
    else :
        return 'NC'

def get_type_of_cameras(l_camera):

    l_camera_type = [None] * 4

    for i in range(0,4):
        k = 2*i
        if l_camera[k] == 'C1' or l_camera[k+1] == 'C1' :
            l_camera_type[i] =  'c1'
        elif l_camera[k] == 'C2' or l_camera[k+1] == 'C2' :
            l_camera_type[i] =  'c2'
        elif l_camera[k] == 'NC' and l_camera[k+1] == 'NC' :
            l_camera_type[i] =  'nc'

    return l_camera_type

def build_symlink_c1c2(dev_num, v_num, camera_type):

    video_num = v_num + 1

    str_subsys  = 'SUBSYSTEM==\"video4linux\", ATTR{name}==\"vi-output, '
    str_symlink = 'SYMLINK+= \"gmsl/tier4-isx021-imx490-cam' + str(video_num) + '\"\n'

    if camera_type == 'C1' :

        driver_name = [ 'tier4_isx021 30-001b\", ',  'tier4_isx021 30-001c\", ', \
                        'tier4_isx021 31-001b\", ',  'tier4_isx021 31-001c\", ', \
                        'tier4_isx021 32-001b\", ',  'tier4_isx021 32-001c\", ', \
                        'tier4_isx021 33-001b\", ',  'tier4_isx021 33-001c\", ' \
                      ]

    elif camera_type == 'C2' :

        driver_name = [ 'tier4_imx490 30-002b\", ', 'tier4_imx490 30-002c\", ', \
                        'tier4_imx490 31-002b\", ', 'tier4_imx490 31-002c\", ', \
                        'tier4_imx490 32-002b\", ', 'tier4_imx490 32-002c\", ', \
                        'tier4_imx490 33-002b\", ', 'tier4_imx490 33-002c\", ' \
                      ]

    str_symlink_dev =  str_subsys + driver_name[v_num] + str_symlink

    return str_symlink_dev

def get_des_number( port_num ):  # port_num is 0 to 7 

    even_port = port_num & 0x6

    if even_port == 0 :
        rc = 12   # port 1,2
    elif even_port == 2 :
        rc = 34   # port 3,4
    elif even_port == 4 :
        rc = 56   # port 5,6
    elif even_port == 6 :
        rc = 78   # port 7,8
    return rc

def usage():
    print('************************************************************************')
    print('**                                                                    **')
    print('**     This is the tool to generate overlay dts and udev file.        **')
    print('**                                                                    **')
    print('************************************************************************')
    print('**                                                                    **')
    print('**  Usage :                                                           **')
    print('**                                                                    **')
    print('** $> python3 make_overaly_dtb2.py camera1  camera2 ... camera8       **')
    print('**                                                                    **')
    print('**  camera1 : camera connected to port1                               **')
    print('**  camera2 : camera connected to port2                               **')
    print('**  camera3 : camera connected to port3                               **')
    print('**      .                                                             **')
    print('**      .                                                             **')
    print('**  camera8 : camera connected to port8                               **')
    print('**                                                                    **')
    print('**  The camerN shoud be c1, c2 or nc(not connected)                   **')
    print('**                                                                    **')
    print('**  Overlay dts will be output to                                     **')
    print('**     \"tier4-isx021-imx490-xx-yy-zz-uu-overlay-device-tree.dts\"      **')
    print('**                                                                    **')
    print('**  udev file will be generated as                                    **')
    print('**    \"99-tier4-isx021-imx490-gmsl.rules\"                             **')
    print('**                                                                    **')
    print('************************************************************************')

def check_last_port(num, camera):

    if num & 0x1 :
        if camera[num] == 'C1' :
            if camera[num-1] == 'C1' or camera[num-1] == 'NC':
                return 0
            else:
                return -1
        elif camera[num] == 'C2' :
            if camera[num-1] == 'C2' or camera[num-1] == 'NC':
                return 0
            else:
                return -1
        elif camera[num] == 'NC' :
            if camera[num-1] == 'C2' or camera[num-1] == 'NC' or camera[num-1] == 'C1':
                return 0
            else:
                return -1
    else:
        return 0

def identify_camera(cmd_arg):
    upper_arg = cmd_arg.upper()
    if  upper_arg == 'C1' :
        return 'C1'
    elif upper_arg == 'C2' :
        return 'C2'
    else :
        return 'NC'

# ----  main ----------

args = sys.argv

#print( 'argv = ' + str(args) )

#print( 'arg len = ' + str(len(args)))

total_num_args = len(args)

found_camera = 0

camera = [  'NC', 'NC', 'NC', 'NC',\
            'NC', 'NC', 'NC', 'NC' ]

temp_cam = [None]*8

if total_num_args != 9 :

    print(' ***** Error! : ' + args[0] + ' needs 8 arguments. *****\n')
    usage()
    sys.exit()

with open( '99-tier4-isx021-imx490-gmsl.rules', 'w', encoding='utf-8') as udev_file:
    for i in range(8):
        temp_cam[i] = identify_camera(args[i+1])
        if temp_cam[i] == 'NC' :
        	camera[i] = 'C1'
        else :
	        camera[i] = temp_cam[i]
        err = check_last_port(i, camera)
        if err == -1  :
            if camera[i] == 'C1' :
                print(' ***** Error! : port[' + str(i-1) + '] should be C1 or NC. *****\n')
            elif camera[i] == 'C2' :
                print(' ***** Error! : port[' + str(i-1) + '] should be C2 or NC. *****\n')
            usage()
            sys.exit()
        else:
            if camera[i] == 'C1' or camera[i] == 'C2' :
                str_line_in_udev = build_symlink_c1c2( i, found_camera, camera[i] )
                udev_file.write(str_line_in_udev)
                found_camera += 1

        if i == 0 :
    
            if camera[i] == 'C1' :
                str_camera1 = str_isx021_a_1b_i2c0
                str_fragment_vi0 = str_fragment_vi_0
                str_fragment_nvcsi0 = str_fragment_nvcsi_ch0.replace('imx490_out0', 'isx021_out0')
                str_cameramodule0 = str_isx021_cameramodule0
            elif camera[i] == 'C2' :
                str_camera1 = str_imx490_a_2b_i2c0
                str_fragment_vi0 = str_fragment_vi_0
                str_fragment_nvcsi0 = str_fragment_nvcsi_ch0
                str_cameramodule0 = str_imx490_cameramodule0
            else:
                str_camera1 = ''
                str_fragment_vi0 = ''
                str_fragment_nvcsi0 = ''
                str_cameramodule0 = ''
    
        elif i == 1 :
    
            if camera[i] == 'C1' :
                str_camera2 = str_isx021_b_1c_i2c0
                str_fragment_vi1 = str_fragment_vi_1
                str_fragment_nvcsi1 = str_fragment_nvcsi_ch1.replace('imx490_out1', 'isx021_out1')
                str_cameramodule1 = str_isx021_cameramodule1
            elif camera[i] == 'C2' :
                str_camera2 = str_imx490_b_2c_i2c0
                str_fragment_vi1 = str_fragment_vi_1
                str_fragment_nvcsi1 = str_fragment_nvcsi_ch1
                str_cameramodule1 = str_imx490_cameramodule1
            else:
                str_camera2 = ''
                str_fragment_vi1 = ''
                str_fragment_nvcsi1 = ''
                str_cameramodule1 = ''

        elif i == 2 :
    
            if camera[i] == 'C1' :
                str_camera3 = str_isx021_c_1b_i2c1
                str_fragment_vi2 = str_fragment_vi_2
                str_fragment_nvcsi2 = str_fragment_nvcsi_ch2.replace('imx490_out2', 'isx021_out2')
                str_cameramodule2 = str_isx021_cameramodule2
            elif camera[i] == 'C2' :
                str_camera3 = str_imx490_c_2b_i2c1
                str_fragment_vi2 = str_fragment_vi_2
                str_fragment_nvcsi2 = str_fragment_nvcsi_ch2
                str_cameramodule2 = str_imx490_cameramodule2
            else:
                str_camera3 = ''
                str_fragment_vi2 = ''
                str_fragment_nvcsi2 = ''
                str_cameramodule2 = ''

            
        elif i == 3 :
    
            if camera[i] == 'C1' :
                str_camera4 = str_isx021_d_1c_i2c1
                str_fragment_vi3 = str_fragment_vi_3
                str_fragment_nvcsi3 = str_fragment_nvcsi_ch3.replace('imx490_out3', 'isx021_out3')
                str_cameramodule3 = str_isx021_cameramodule3
            elif camera[i] == 'C2' :
                str_camera4 = str_imx490_d_2c_i2c1
                str_fragment_vi3 = str_fragment_vi_3
                str_fragment_nvcsi3 = str_fragment_nvcsi_ch3
                str_cameramodule3 = str_imx490_cameramodule3
            else:
                str_camera4 = ''
                str_fragment_vi3 = ''
                str_fragment_nvcsi3 = ''
                str_cameramodule3 = ''

        elif i == 4 :
    
            if camera[i] == 'C1' :
                str_camera5 = str_isx021_e_1b_i2c2
                str_fragment_vi4 = str_fragment_vi_4
                str_fragment_nvcsi4 = str_fragment_nvcsi_ch4.replace('imx490_out4', 'isx021_out4')
                str_cameramodule4 = str_isx021_cameramodule4
            elif camera[i] == 'C2' :
                str_camera5 = str_imx490_e_2b_i2c2
                str_fragment_vi4 = str_fragment_vi_4
                str_fragment_nvcsi4 = str_fragment_nvcsi_ch4
                str_cameramodule4 = str_imx490_cameramodule4
            else:
                str_camera5 = ''
                str_fragment_vi4 = ''
                str_fragment_nvcsi4 = ''
                str_cameramodule4 = ''
    
        elif i == 5 :
    
            if camera[i] == 'C1' :
                str_camera6 = str_isx021_f_1c_i2c2
                str_fragment_vi5 = str_fragment_vi_5
                str_fragment_nvcsi5 = str_fragment_nvcsi_ch5.replace('imx490_out5', 'isx021_out5')
                str_cameramodule5 = str_isx021_cameramodule5
            elif camera[i] == 'C2' :
                str_camera6 = str_imx490_f_2c_i2c2
                str_fragment_vi5 = str_fragment_vi_5
                str_fragment_nvcsi5 = str_fragment_nvcsi_ch5
                str_cameramodule5 = str_imx490_cameramodule5
            else:
                str_camera6 = ''
                str_fragment_vi5 = ''
                str_fragment_nvcsi5 = ''
                str_cameramodule5 = ''
    
        elif i == 6 :
    
            if camera[i] == 'C1' :
                str_camera7 = str_isx021_g_1b_i2c3
                str_fragment_vi6 = str_fragment_vi_6
                str_fragment_nvcsi6 = str_fragment_nvcsi_ch6.replace('imx490_out6', 'isx021_out6')
                str_cameramodule6 = str_isx021_cameramodule6
            elif camera[i] == 'C2' :
                str_camera7 = str_imx490_g_2b_i2c3
                str_fragment_vi6 = str_fragment_vi_6
                str_fragment_nvcsi6 = str_fragment_nvcsi_ch6
                str_cameramodule6 = str_imx490_cameramodule6
            else:
                str_camera7 = ''
                str_fragment_vi6 = ''
                str_fragment_nvcsi6 = ''
                str_cameramodule6 = ''

        elif i == 7 :
    
            if camera[i] == 'C1' :
                str_camera8 = str_isx021_h_1c_i2c3
                str_fragment_vi7 = str_fragment_vi_7
                str_fragment_nvcsi7 = str_fragment_nvcsi_ch7.replace('imx490_out7', 'isx021_out7')
                str_cameramodule7 = str_isx021_cameramodule7
            elif camera[i] == 'C2' :
                str_camera8 = str_imx490_h_2c_i2c3
                str_fragment_vi7 = str_fragment_vi_7
                str_fragment_nvcsi7 = str_fragment_nvcsi_ch7
                str_cameramodule7 = str_imx490_cameramodule7
            else:
                str_camera8 = ''
                str_fragment_vi7 = ''
                str_fragment_nvcsi7 = ''
                str_cameramodule7 = ''

str_i2c0 = str_fragment0 + str_ser_isp_i2c0 + str_camera2 + str_camera1 +  str_block_end

str_i2c1 = str_fragment1 + str_ser_isp_i2c1 + str_camera4 + str_camera3 +  str_block_end

str_i2c2 = str_fragment2 + str_ser_isp_i2c2 + str_camera6 + str_camera5 +  str_block_end

str_i2c3 = str_fragment3 + str_ser_isp_i2c3 + str_camera8 + str_camera7 +  str_block_end

str_whole_i2c = str_i2c0 + str_i2c1 + str_i2c2 + str_i2c3

str_whole_dts = str_overlay_header \
                + str_i2c0 + str_i2c1 + str_i2c2 + str_i2c3 \
                + str_fragment_vi0 + str_fragment_vi1 + str_fragment_vi2 + str_fragment_vi3 \
                + str_fragment_vi4 + str_fragment_vi5 + str_fragment_vi6 + str_fragment_vi7 \
                + str_fragment_nvcsi0 + str_fragment_nvcsi1 + str_fragment_nvcsi2 + str_fragment_nvcsi3 \
                + str_fragment_nvcsi4 + str_fragment_nvcsi5 + str_fragment_nvcsi6 + str_fragment_nvcsi7 \
                + str_cameramodule0 + str_cameramodule1 + str_cameramodule2 + str_cameramodule3 \
                + str_cameramodule4 + str_cameramodule5 + str_cameramodule6 + str_cameramodule7 \
                + str_dser_fragment + str_overlay_end

camera_type = get_type_of_cameras(temp_cam)
#camera_type = get_type_of_cameras(camera)

overlay_dts_file_name = 'tier4-isx021-imx490-' + str(camera_type[0]) + '-' + str(camera_type[1]) + '-' \
                        + str(camera_type[2]) + '-' + str(camera_type[3]) + '-gmsl-device-tree-overlay.dts'

with open( overlay_dts_file_name, 'w', encoding='utf-8') as overlay_dts_file:
    print(str_whole_dts, file=overlay_dts_file )

