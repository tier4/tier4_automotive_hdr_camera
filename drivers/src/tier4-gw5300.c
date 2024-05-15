/*
 * tier4_gw5300.c - tier4_gw5300 ISP driver
 *
 * Copyright (c) 2022-2023, TIER IV Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <linux/module.h>
#include <linux/string.h>
#include <media/camera_common.h>

#include "tier4-gmsl-link.h"
#include "tier4-gw5300.h"

struct tier4_gw5300
{
  struct i2c_client *i2c_client;
  struct tier4_gmsl_link_ctx g_client;
  struct mutex lock;
  /* primary ISP properties */
  __u32 def_addr;
};

#define MAX_CHANNEL_NUM 8

#define NO_ERROR 0

/* count channel,the max MAX_CHANNEL_NUM*/
static __u32 channel_count_gw5300;

static struct tier4_gw5300 *prim_priv__[MAX_CHANNEL_NUM];

static u8 master_30fps[] = {
                             0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                           , 0x80, 0x03, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00
                           , 0x00, 0x6A
};

static u8 master_10fps[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x03, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00
                          , 0x00, 0x38
};

static u8 slave_10fps[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x03, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00
                          , 0x00, 0x42
};

static u8 master_20fps[] = {
                             0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                           , 0x80, 0x03, 0x00, 0x00, 0x00, 0x5A, 0x00, 0x00
                           , 0x00, 0x74
};

static u8 slave_20fps[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x03, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00
                          , 0x00, 0x79
};

static u8 slave_30fps[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x03, 0x00, 0x00, 0x00, 0x55, 0x00, 0x00
                          , 0x00, 0x6F
};

static u8 master_10fps_slow[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                          , 0x00, 0x17
};

static u8 slave_10fps_slow[] = {
                            0x33, 0x47, 0x0B, 0x00, 0x00, 0x00, 0x12, 0x00
                          , 0x80, 0x03, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x00
                          , 0x00, 0x65
};


/*
 * C3 camera mode support
 */
struct mode_command {
    u8 *command;
    size_t len;
    unsigned int delay_ms;
};

#define MODE_CMD(cmd, delay) \
    { \
        .command = (cmd), \
        .len = sizeof (cmd), \
        .delay_ms = delay, \
    }

#define NUM_VA_ARGS(type, ...)  (sizeof ((type[]) {__VA_ARGS__}) / sizeof (type))

#define MODE_SEQ(h_line_time_ns, ...) \
    { \
        .commands = (struct mode_command[]){ \
            __VA_ARGS__ \
        }, \
        .len = NUM_VA_ARGS(struct mode_command, __VA_ARGS__), \
        .h_line_ns = (h_line_time_ns) \
    }

struct mode_sequence {
    struct mode_command *commands;
    size_t len;
    u32 h_line_ns;
};

/*
 * C3 preset modes
 */

static u8 c3_master_5fps_cmd[] = {
    0x33, 0x47, 0xb, 0x0, 0x0, 0x0, 0x12, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x17
};

/* mode 0 */
static struct mode_sequence c3_master_5fps_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_5fps_cmd, 0),
    );

/* mode 4 */
static u8 c3_master_20fps_cmd[] = {
    0x33, 0x47, 0xb, 0x0, 0x0, 0x0, 0x12, 0x0, 0x80, 0x3, 0x0, 0x0, 0x0, 0x4b, 0x0, 0x0, 0x0, 0x65
};
static struct mode_sequence c3_master_20fps_seq =
    MODE_SEQ(
        20000,
        MODE_CMD(c3_master_20fps_cmd, 0)
    );

static u8 c3_master_20fps_ebd_cmd[] = {
    0x33, 0x47, 0xb, 0x0, 0x0, 0x0, 0x12, 0x0, 0x80, 0x3, 0x0, 0x0, 0x0, 0x50, 0x0, 0x0, 0x0, 0x6a
};

/* mode 10 */
static struct mode_sequence c3_master_20fps_ebd_seq =
    MODE_SEQ(
        20000,
        MODE_CMD(c3_master_20fps_ebd_cmd, 0)
    );

static u8 c3_master_30fps_cmd[] = {
    0x33, 0x47, 0xb, 0x0, 0x0, 0x0, 0x12, 0x0, 0x80, 0x3, 0x0, 0x0, 0x0, 0x1e, 0x0, 0x0, 0x0, 0x38
};

/* mode 6 */
static struct mode_sequence c3_master_30fps_seq =
    MODE_SEQ(
        13890,
        MODE_CMD(c3_master_30fps_cmd, 0)
    );

static u8 c3_master_30fps_ebd_cmd[] = {
    0x33, 0x47, 0xb, 0x0, 0x0, 0x0, 0x12, 0x0, 0x80, 0x3, 0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x42
};

/* mode 12 */
static struct mode_sequence c3_master_30fps_ebd_seq =
    MODE_SEQ(
        13890,
        MODE_CMD(c3_master_30fps_ebd_cmd, 0)
    );


/*
 * C3 derived modes
 */
static u8 c3_regmap_ffff[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x25
};

static u8 c3_standby_mode[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x4, 0x1b, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x2, 0x1, 0x45
};

static u8 c3_drive_mode_sel[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x40, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0x68
};

static u8 c3_master_sync_mode[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x41, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x2, 0x1, 0x6d
};

static u8 c3_sensor_streaming[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x4, 0x1b, 0x0, 0x0, 0x5c, 0x0, 0x0, 0x0, 0x2, 0x1, 0xa2
};

static u8 c3_master_sync_mode_2[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x4, 0x1b, 0x0, 0x0, 0xa3, 0x0, 0x0, 0x0, 0x2, 0x1, 0xe9
};

/* mode 1 */
static struct mode_sequence c3_slave_5fps_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_5fps_cmd, 5000),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

static u8 c3_20_to_10fps_cmd_1[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x50, 0x97, 0x0, 0x0, 0xc4, 0x0, 0x0, 0x0, 0x2, 0x1, 0xd2,
};

static u8 c3_20_to_10fps_cmd_2[] = {
    0x33, 0x47, 0x15, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x80, 0x1, 0x0, 0x0, 0x0, 0x34, 0x0, 0x0,
    0x0, 0x51, 0x97, 0x0, 0x0, 0x09, 0x0, 0x0, 0x0, 0x2, 0x1, 0x18,
};

/* mode 2 */
static struct mode_sequence c3_master_10fps_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_20fps_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),
    );

/* mode 3 */
static struct mode_sequence c3_slave_10fps_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_20fps_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

/* mode 5 */
static struct mode_sequence c3_slave_20fps_seq =
    MODE_SEQ(
        20000,
        MODE_CMD(c3_master_20fps_cmd, 5000),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

/* mode 7 */
static struct mode_sequence c3_slave_30fps_seq =
    MODE_SEQ(
        13890,
        MODE_CMD(c3_master_30fps_cmd, 5000),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

/* mode 8 */
static struct mode_sequence c3_master_10fps_ebd_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),
    );

/* mode 9 */
static struct mode_sequence c3_slave_10fps_ebd_seq =
    MODE_SEQ(
        40000,
        MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

/* mode 11 */
static struct mode_sequence c3_slave_20fps_ebd_seq =
    MODE_SEQ(
        20000,
        MODE_CMD(c3_master_20fps_ebd_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

/* mode 13 */
static struct mode_sequence c3_slave_30fps_ebd_seq =
    MODE_SEQ(
        13890,
        MODE_CMD(c3_master_30fps_ebd_cmd, 5000),

        MODE_CMD(c3_20_to_10fps_cmd_1, 2000),
        MODE_CMD(c3_20_to_10fps_cmd_2, 500),

        MODE_CMD(c3_regmap_ffff, 500),
        MODE_CMD(c3_standby_mode, 500),
        MODE_CMD(c3_drive_mode_sel, 500),
        MODE_CMD(c3_master_sync_mode, 500),
        MODE_CMD(c3_sensor_streaming, 500),
        MODE_CMD(c3_master_sync_mode_2, 500),
    );

static struct mode_sequence c3_mode_seqs[GW5300_MODE_MAX];


struct map_ctx
{
  u8 dt;
  u16 addr;
  u8 val;
  u8 st_id;
};

#if 0
static int tier4_gw5300_receive_msg(struct device *dev, u8 *data, int data_size )
{
    int err = 0;
    struct i2c_msg msg;
    struct tier4_gw5300 *priv = dev_get_drvdata(dev);

    msg.addr = priv->i2c_client->addr;
    msg.flags = I2C_M_RD;           // I2C Read
    msg.len = data_size;
    msg.buf = data;

    err = i2c_transfer(priv->i2c_client->adapter, &msg, 1);

    if (err <= 0 ) {
        dev_err(dev, "[%s] : i2c_transer receive message failed : slave addr = 0x%x\n", __func__, msg.addr );
    }

    return err;
}
#endif

// -------------------------------------------------------------------

static int tier4_gw5300_send_and_recv_msg(struct device *dev, u8 *wdata, int wdata_size, u8 *rdata, int rdata_size)
{
  int err = 0;
  struct i2c_msg msg[2];
  struct tier4_gw5300 *priv = dev_get_drvdata(dev);

  msg[0].addr = priv->i2c_client->addr;
  msg[0].flags = 0;  // I2C Write
  msg[0].len = wdata_size;
  msg[0].buf = wdata;

  msg[1].addr = priv->i2c_client->addr;
  msg[1].flags = I2C_M_RD;  // I2C Read
  msg[1].len = rdata_size;
  msg[1].buf = rdata;

  err = i2c_transfer(priv->i2c_client->adapter, msg, 2);

  if (err <= 0)
  {
    dev_err(dev, "[%s] : i2c_transer send message failed. %d: slave addr = 0x%x\n", __func__, err, msg[0].addr);
  }

  return err;  //  the total number of bytes to have been sent or recived
}

// -------------------------------------------------------------------

static int tier4_gw5300_c3_send_and_recv_msg(struct device *dev, u8 *wdata, int wdata_size, u8 *rdata, int rdata_size)
{
  return tier4_gw5300_send_and_recv_msg(dev, wdata, wdata_size, rdata, rdata_size);
}

static int tier4_gw5300_mode_seq_send_and_recv_msg(struct device *dev, struct mode_sequence *mode_seq, u8 *rdata, int rdata_size)
{
  int i;
  int err = 0;

  for (i = 0; i < mode_seq->len; ++i) {
    struct mode_command *cmd = &mode_seq->commands[i];

    err = tier4_gw5300_send_and_recv_msg(dev, cmd->command, cmd->len, rdata, rdata_size);
    msleep(cmd->delay_ms);
    if (err < 0) {
        dev_err(dev, "%s: Failed to send a command[%d]: %d\n", __func__, i, err);
        break;
    }
  }


  return err;
}

// -------------------------------------------------------------------

uint8_t calcCheckSum(const uint8_t *data, size_t size){
    uint8_t result = 0;
    size_t i =0;
      for(i=0; i<size; i++){
          result += data[i];
      }
      return result;
}

int tier4_gw5300_set_integration_time_on_aemode(struct device *dev, u32 h_line_ns, u32 max_integration_time, u32 min_integration_time){
  u8    buf[6];
  int ret = 0;

  u8 cmd_integration_max[22] = {0x33, 0x47, 0x0f, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05, 0x00, 0x15, 0x00, 0x01, 0x00, 0x04, 0x00, 0x70, 0x03, 0x00, 0x00, 0x00}; // val = 0x70, 0x03, 0x00, 0x00
  u8 cmd_integration_min[20] = {0x33, 0x47, 0x0d, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05, 0x00, 0x21, 0x00, 0x01, 0x00, 0x02, 0x00, 0x70, 0x03, 0x00}; //val = 0x70, 0x03

  const size_t max_val_pos = 17;
  const size_t min_val_pos = 17;
  uint32_t min_line = DIV_ROUND_CLOSEST(min_integration_time * 1000, h_line_ns);
  uint32_t max_line = DIV_ROUND_CLOSEST(max_integration_time * 1000, h_line_ns);
  uint8_t b1 = max_line &0xFF;
  uint8_t b2 = (max_line >> 8)&0xFF;
  uint8_t b3 = 0;
  uint8_t b4 = 0;

  dev_info(dev, "%s: integration time(%u[ns/H line]): max=(%d[us], %d[line]) min=(%d[us], %d[line])\n",
          __func__, h_line_ns, max_integration_time, max_line, min_integration_time, min_line);

  cmd_integration_max[max_val_pos] = b1;
  cmd_integration_max[max_val_pos+1] = b2;
  cmd_integration_max[max_val_pos+2] = b3;
  cmd_integration_max[max_val_pos+3] = b4;
  cmd_integration_max[sizeof(cmd_integration_max)-1] = calcCheckSum(cmd_integration_max, sizeof(cmd_integration_max));


  b1 = min_line &0xFF;
  b2 = (min_line >> 8)&0xFF;

  cmd_integration_min[min_val_pos] = b1;
  cmd_integration_min[min_val_pos+1] = b2;

  cmd_integration_min[sizeof(cmd_integration_min)-1] = calcCheckSum(cmd_integration_min, sizeof(cmd_integration_min));


    msleep(20);
  ret += tier4_gw5300_send_and_recv_msg(dev, cmd_integration_max, sizeof(cmd_integration_max), buf, sizeof(buf));
    msleep(20);
  ret += tier4_gw5300_send_and_recv_msg(dev, cmd_integration_min, sizeof(cmd_integration_min), buf, sizeof(buf));

  return ret;

}

int tier4_gw5300_c2_set_integration_time_on_aemode(struct device *dev, int trigger_mode, u32 max_integration_time, u32 min_integration_time)
{
    size_t h_line_ns = 12500;

    if (trigger_mode == GW5300_MASTER_MODE_10FPS_SLOW ||
            trigger_mode == GW5300_SLAVE_MODE_10FPS_SLOW)
        h_line_ns = 50000;

    return tier4_gw5300_set_integration_time_on_aemode(dev, h_line_ns, max_integration_time, min_integration_time);
}
EXPORT_SYMBOL(tier4_gw5300_c2_set_integration_time_on_aemode);


int tier4_gw5300_c3_set_integration_time_on_aemode(struct device *dev, int trigger_mode, u32 max_integration_time, u32 min_integration_time)
{
    return tier4_gw5300_set_integration_time_on_aemode(dev, c3_mode_seqs[trigger_mode].h_line_ns,
            max_integration_time, min_integration_time);
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_integration_time_on_aemode);

// ------------------------------------------------------------------

int tier4_gw5300_set_distortion_correction(struct device *dev, bool val)
{
int ret = 0;
  u8  buf[6];
  u8 cmd_dwp_on[]={0x33, 0x47, 0x06, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x80, 0x04, 0x00, 0x01, 0x52};
  u8 cmd_dwp_off[]={0x33, 0x47, 0x03, 0x00, 0x00, 0x00, 0x45, 0x00, 0x80, 0x42};

  if(val){
    ret += tier4_gw5300_send_and_recv_msg(dev, cmd_dwp_on, sizeof(cmd_dwp_on), buf, sizeof(buf));
  }else{
    ret += tier4_gw5300_send_and_recv_msg(dev, cmd_dwp_off, sizeof(cmd_dwp_off), buf, sizeof(buf));
  }
  return ret;
}
EXPORT_SYMBOL(tier4_gw5300_set_distortion_correction);

// ------------------------------------------------------------------

int tier4_gw5300_c3_set_distortion_correction(struct device *dev, bool val)
{
int ret = 0;
  u8  buf[6];
  u8 cmd_dwp_on[]   ={ 0x33, 0x47, 0x6, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x80, 0x04, 0x00, 0x01, 0x52};
  u8 cmd_dwp_off[]  ={ 0x33, 0x47, 0x3, 0x00, 0x00, 0x00, 0x45, 0x00, 0x80, 0x042};

  if(val){
    ret += tier4_gw5300_c3_send_and_recv_msg(dev, cmd_dwp_on, sizeof(cmd_dwp_on), buf, sizeof(buf));
  }else{
    ret += tier4_gw5300_c3_send_and_recv_msg(dev, cmd_dwp_off, sizeof(cmd_dwp_off), buf, sizeof(buf));
  }
  return ret;
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_distortion_correction);

// ------------------------------------------------------------------

int tier4_gw5300_c3_set_auto_exposure(struct device *dev, bool val)
{
  int ret = 0;
  u8  buf[6];
  u8 cmd_auto_exp_on[]  ={ 0x33, 0x47, 0x0C, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05, 0x00, 0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x69 };
  u8 cmd_auto_exp_off[] ={ 0x33, 0x47, 0x0C, 0x00, 0x00, 0x00, 0x55, 0x00, 0x80, 0x05, 0x00, 0x07, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x6A };

  dev_info(dev, "%s: auto exposure is %s\n", __func__, val ? "on" : "off");

  if (val) {
    ret += tier4_gw5300_c3_send_and_recv_msg(dev, cmd_auto_exp_on, sizeof(cmd_auto_exp_on), buf, sizeof(buf));
  } else {
    ret += tier4_gw5300_c3_send_and_recv_msg(dev, cmd_auto_exp_off, sizeof(cmd_auto_exp_off), buf, sizeof(buf));
  }
  return ret;
}
EXPORT_SYMBOL(tier4_gw5300_c3_set_auto_exposure);

// ------------------------------------------------------------------

int tier4_gw5300_check_device(struct device *dev, u8 *rdata, int rdata_size )
{
  int err = 0;
  struct i2c_msg msg[2];
  struct tier4_gw5300 *priv = dev_get_drvdata(dev);

  msg[0].addr = priv->i2c_client->addr;
  msg[0].flags = I2C_M_RD;  // I2C Read
  msg[0].len = rdata_size;
  msg[0].buf = rdata;

  err = i2c_transfer(priv->i2c_client->adapter, msg, 1);

  if (err <= 0)
  {
    dev_err(dev, "[%s] : i2c_transer send message failed. : slave addr = 0x%x\n", __func__, msg[0].addr);
  }
  else
  {
    err = NO_ERROR;
  }

  return err;
}
EXPORT_SYMBOL(tier4_gw5300_check_device);

int tier4_gw5300_prim_slave_addr(struct tier4_gmsl_link_ctx *g_ctx)
{
  if (!g_ctx)
  {
    dev_err(&prim_priv__[channel_count_gw5300 - 1]->i2c_client->dev, "[%s] : Failed. g_ctx is null\n", __func__);
    return -1;
  }

  g_ctx->sdev_isp_def = prim_priv__[channel_count_gw5300 - 1]->def_addr;

  return 0;
}
EXPORT_SYMBOL(tier4_gw5300_prim_slave_addr);

int tier4_gw5300_setup_sensor_mode(struct device *dev, int sensor_mode)
{
  int err = 0;
  u8 buf[6];

  memset(buf, 0x00, 6);

  switch (sensor_mode)
  {
    case GW5300_MASTER_MODE_10FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, master_10fps, sizeof(master_10fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Master mode 10fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;

    case GW5300_SLAVE_MODE_10FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, slave_10fps, sizeof(slave_10fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n", __func__, err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Slave mode 10fps failed. %d message has been sent to gw5300.\n", __func__, err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    case GW5300_MASTER_MODE_20FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, master_20fps, sizeof(master_20fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Master mode 20fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    case GW5300_SLAVE_MODE_20FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, slave_20fps, sizeof(slave_20fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n", __func__, err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Slave mode 20fps failed. %d message has been sent to gw5300.\n", __func__, err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    case GW5300_MASTER_MODE_30FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, master_30fps, sizeof(master_30fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Master mode 30fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;

    case GW5300_SLAVE_MODE_30FPS:
      err = tier4_gw5300_send_and_recv_msg(dev, slave_30fps, sizeof(slave_30fps), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n", __func__, err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Slave mode 30fps failed. %d message has been sent to gw5300.\n", __func__, err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    case GW5300_MASTER_MODE_10FPS_SLOW:
      err = tier4_gw5300_send_and_recv_msg(dev, master_10fps_slow, sizeof(master_10fps_slow), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Slow clock Master mode 10fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Slow clock Master mode 10fps failed. %d message has been sent to gw5300.\n", __func__,
                err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    case GW5300_SLAVE_MODE_10FPS_SLOW:
      err = tier4_gw5300_send_and_recv_msg(dev, slave_10fps_slow, sizeof(slave_10fps_slow), buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up Slow clock Slave mode 10fps failed. %d message has been sent to gw5300.\n", __func__, err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up Slow clock Slave mode 10fps failed. %d message has been sent to gw5300.\n", __func__, err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    default:
      break;
  }

error:

  return err;
}
EXPORT_SYMBOL(tier4_gw5300_setup_sensor_mode);

// -----------   for C3 camera   ----------

int tier4_gw5300_c3_setup_sensor_mode(struct device *dev, int sensor_mode)
{
  int err = 0;
  u8 buf[6];

  memset(buf, 0x00, 6);

  switch (sensor_mode)
  {
    case GW5300_MASTER_MODE_10FPS:
    case GW5300_SLAVE_MODE_10FPS:
    case GW5300_MASTER_MODE_20FPS:
    case GW5300_SLAVE_MODE_20FPS:
    case GW5300_MASTER_MODE_30FPS:
    case GW5300_SLAVE_MODE_30FPS:
    case GW5300_MASTER_MODE_5FPS:
    case GW5300_SLAVE_MODE_5FPS:
    case GW5300_MASTER_MODE_10FPS_EBD:
    case GW5300_SLAVE_MODE_10FPS_EBD:
    case GW5300_MASTER_MODE_20FPS_EBD:
    case GW5300_SLAVE_MODE_20FPS_EBD:
    case GW5300_MASTER_MODE_30FPS_EBD:
    case GW5300_SLAVE_MODE_30FPS_EBD:
      err = tier4_gw5300_mode_seq_send_and_recv_msg(dev, &c3_mode_seqs[sensor_mode], buf, sizeof(buf));
      if (err < 0)
      {
        dev_err(dev, "[%s] : Setting up %s failed. %d message has been sent to gw5300.\n", __func__,
                gw5300_mode_name[sensor_mode], err);
        goto error;
      }
      else if (err == 0)
      {  // it means that 0 message has been sent.
        dev_err(dev, "[%s] : Setting up %s failed. %d message has been sent to gw5300.\n", __func__,
                gw5300_mode_name[sensor_mode], err);
        err = -999;
        goto error;
      }
      else
      {
        err = 0;
      }
      break;
    default:
      break;
  }

error:

  return err;
}
EXPORT_SYMBOL(tier4_gw5300_c3_setup_sensor_mode);


static int tier4_gw5300_probe(struct i2c_client *client,
                const struct i2c_device_id *id)
{
  struct tier4_gw5300 *priv;
  int err = 0;
  struct device_node *node = client->dev.of_node;

  dev_info(&client->dev, "[%s] : probing GW5300 ISP\n", __func__);

  priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
  priv->i2c_client = client;

  mutex_init(&priv->lock);

  if (of_get_property(node, "is-prim-isp", NULL))
  {
    if (prim_priv__[channel_count_gw5300] && channel_count_gw5300 >= MAX_CHANNEL_NUM)
    {
      dev_err(&client->dev, "[%s] : prim-isp already exists\n", __func__);
      return -EEXIST;
    }

    err = of_property_read_u32(node, "reg", &priv->def_addr);
    if (err < 0)
    {
      dev_err(&client->dev, "[%s] : reg not found\n", __func__);
      return -EINVAL;
    }

    prim_priv__[channel_count_gw5300] = priv;
    channel_count_gw5300++;
  }

  dev_set_drvdata(&client->dev, priv);

  /* dev communication gets validated when GMSL link setup is done */
  dev_info(&client->dev, "[%s] :  Probing succeeded\n", __func__);

  return err;
}

static int tier4_gw5300_remove(struct i2c_client *client)
{
  struct tier4_gw5300 *priv;

  if (channel_count_gw5300 > 0)
    channel_count_gw5300--;

  if (client != NULL)
  {
    priv = dev_get_drvdata(&client->dev);
    mutex_destroy(&priv->lock);
  }

  return 0;
}

static const struct i2c_device_id tier4_gw5300_id[] = {
  { "tier4_gw5300", 0 },
  {},
};

const struct of_device_id tier4_gw5300_of_match[] = {
  {
      .compatible = "nvidia,tier4_gw5300",
  },
  {},
};
MODULE_DEVICE_TABLE(of, tier4_gw5300_of_match);
MODULE_DEVICE_TABLE(i2c, tier4_gw5300_id);

static struct i2c_driver tier4_gw5300_i2c_driver = {
    .driver = {
        .name = "tier4_gw5300",
        .owner = THIS_MODULE,
    },
    .probe = tier4_gw5300_probe,
    .remove = tier4_gw5300_remove,
    .id_table = tier4_gw5300_id,
};

static int __init tier4_gw5300_init(void)
{
  printk(KERN_INFO "ISP Driver for TIERIV Camera.\n");

  c3_mode_seqs[GW5300_MASTER_MODE_5FPS] = c3_master_5fps_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_5FPS] = c3_slave_5fps_seq;

  c3_mode_seqs[GW5300_MASTER_MODE_10FPS] = c3_master_10fps_seq;
  c3_mode_seqs[GW5300_MASTER_MODE_10FPS_EBD] = c3_master_10fps_ebd_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_10FPS] = c3_slave_10fps_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_10FPS_EBD] = c3_slave_10fps_ebd_seq;

  c3_mode_seqs[GW5300_MASTER_MODE_20FPS] = c3_master_20fps_seq;
  c3_mode_seqs[GW5300_MASTER_MODE_20FPS_EBD] = c3_master_20fps_ebd_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_20FPS] = c3_slave_20fps_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_20FPS_EBD] = c3_slave_20fps_ebd_seq;

  c3_mode_seqs[GW5300_MASTER_MODE_30FPS] = c3_master_30fps_seq;
  c3_mode_seqs[GW5300_MASTER_MODE_30FPS_EBD] = c3_master_30fps_ebd_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_30FPS] = c3_slave_30fps_seq;
  c3_mode_seqs[GW5300_SLAVE_MODE_30FPS_EBD] = c3_slave_30fps_ebd_seq;

  return i2c_add_driver(&tier4_gw5300_i2c_driver);
}

static void __exit tier4_gw5300_exit(void)
{
  i2c_del_driver(&tier4_gw5300_i2c_driver);
}

module_init(tier4_gw5300_init);
module_exit(tier4_gw5300_exit);

MODULE_DESCRIPTION("GW5300 ISP driver tier4_gw5300");
MODULE_AUTHOR("Kohji Iwasaki");
MODULE_LICENSE("GPL v2");
