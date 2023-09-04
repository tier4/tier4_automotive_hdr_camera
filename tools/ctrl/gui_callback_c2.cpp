#include <t4cam_tools.hpp>
#include <iostream>

#include "gui.h"

/*
 * Callback
 */



void SampleWindowC2::callback_quit(void)
{
  this->app->quit();
}

void SampleWindowC2::callback_default_button(void)
{
  std::cerr << "Not supported function" << std::endl;
}

void SampleWindowC2::callback_load_button(void)
{
  std::cerr << "Not supported function" << std::endl;
}

void SampleWindowC2::save_callback()
{
  std::cerr << "Not supported function" << std::endl;
}

void SampleWindowC2::callback_radio(int i)
{
  if ((int)ae_radio[i].get_active() == 1)
  {
    for (int j = 0; j < 8; j++)
    {
      if (available_mask & (1 << j))
      {
        if(i == 0)
          camera_ptr_array[j]->setAutoExposure(true);
        else  
          camera_ptr_array[j]->setAutoExposure(false);
      }
    }
    if (i != 3)
    {
      shutter_speed_scale.set_sensitive(true);
      isp_sensor_gain_scale.set_sensitive(false);
      sensor_gain_scale.set_sensitive(false);

    }
    else
    {
      shutter_speed_scale.set_sensitive(false);
      isp_sensor_gain_scale.set_sensitive(true);
      sensor_gain_scale.set_sensitive(true);
    }
  }
}

/////


void SampleWindowC2::callback_check(int i)
{
	fprintf(stderr,"%s:%d\n", __func__, i);
  if (buttons[i].get_active() == 0)
  {
	fprintf(stderr,"off_%s:%d\n", __func__, i);
    available_mask &= (~(1 << i)) & master_available_mask;
  }
  else
  {
	fprintf(stderr,"on_%s:%d\n", __func__, i);
    available_mask |= (1 << i) & master_available_mask;
  }
}

// image tuning
void SampleWindowC2::brightness_callback_scale(double v)
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setBrightnessVal(brightness_scale.get_value());
    }
  }
}
void SampleWindowC2::hue_callback_scale(double v)
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setHueVal(hue_scale.get_value());
    }
  }
}
void SampleWindowC2::saturation_callback_scale(double v)
{
	saturation_val = (int)v;

#if 0 // exec on timer function 
	exec_available_cam<bool>(&C2::setSaturation, false);
	exec_available_cam<int>(&C2::setSaturationVal, int(saturation_scale.get_value()));
	exec_available_cam<bool>(&C2::setSaturation, true);
#endif
}
void SampleWindowC2::contrast_callback_scale(double v)
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setContrastVal(contrast_scale.get_value());
    }
  }
}

void SampleWindowC2::sharpness_callback_scale(double v)
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setSharpness(true);
    }
  }
}
