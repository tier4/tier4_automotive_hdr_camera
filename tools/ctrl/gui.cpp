#include "gui.h"

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv);
  SampleWindow window(640, 480);
  return app->run(window);
}

void scaleConfig(Gtk::Scale &scale)
{
  scale.set_range(0, 256);
  scale.set_value(128);
  scale.set_digits(2);
  scale.set_increments(0.1, 1);
  scale.set_draw_value(true);
  scale.set_value_pos(Gtk::POS_RIGHT);
  // scale.set_size_request(250);
}

void scaleConfig(Gtk::Scale &scale, float min, float max, float init)
{
  scale.set_range(min, max);
  scale.set_value(init);
  scale.set_digits(4);
  scale.set_increments(0.1, 1);
  scale.set_draw_value(true);
  scale.set_value_pos(Gtk::POS_RIGHT);
  // scale.set_size_request(250);
}

// Frame
//
void SampleWindow::createSelectPortFrame(void)
{
  // port Frame

  for (int i = 0; i < 8; i++)
  {
    buttons[i].set_label(std::to_string(i));
    buttons[i].set_active(i == 0);
    h_check_box.pack_start(buttons[i]);
    buttons[i].signal_toggled().connect([this, i] { this->callback_check(i); });

    if (!(available_mask & (1 << i)))
    {
      buttons[i].set_sensitive(false);
    }
  }
  select_port_frame.set_label("select ports");
  select_port_frame.add(h_check_box);
}

void SampleWindow::createAEFrame(void)
{
  // get digital gain
  float digital_gain = 30;
  scaleConfig(digital_gain_scale, 0.0, 1023, digital_gain);
  digital_gain_scale.set_sensitive(false);

  ae_radio[0].set_label("AE(Auto)");
  ae_radio[1].set_label("AE(Hold)");
  ae_radio[1].set_sensitive(false);

  ae_radio[2].set_label("Scale ME");
  ae_radio[2].set_sensitive(false);
  ae_radio[3].set_label("Full ME");
  ae_radio[0].set_active(true);

  Gtk::RadioButton::Group ae_radio_group = ae_radio[0].get_group();
  ae_radio[1].set_group(ae_radio_group);
  ae_radio[2].set_group(ae_radio_group);
  ae_radio[3].set_group(ae_radio_group);

  for (int i = 0; i < 4; i++)
  {
    ae_radio_box.pack_start(ae_radio[i]);
    ae_radio[i].signal_toggled().connect([this, i] { this->callback_radio(i); });
  }

  // evref

  evref_label.set_label("evref_offset");
  scaleConfig(evref_scale, -12.61, 12.7, 0);

  scaleConfig(shutter_speed_scale, 0, 1000, 11);
  shutter_speed_scale.set_sensitive(false);
  shutter_speed_label.set_label("shutter_speed_fme");

  ae_grid.set_column_homogeneous(true);
  ae_grid.attach(ae_radio_box, 0, 0, 3, 1);
  ae_grid.attach(digital_gain_label, 0, 1, 1, 1);
  ae_grid.attach(digital_gain_scale, 1, 1, 2, 1);
  ae_grid.attach(shutter_speed_label, 0, 2, 1, 1);
  ae_grid.attach(shutter_speed_scale, 1, 2, 2, 1);

  shutter_speed_scale.signal_value_changed().connect([this] {
    for (int i = 0; i < 8; i++)
    {
      if (this->available_mask & (1 << i))
      {
        this->camera_ptr_array[i]->setShutterSpeedforFME(this->shutter_speed_scale.get_value());
      }
    }
  });

  evref_check.set_label("evref_offset_enable");
  evref_check.set_active(false);
  evref_check.signal_toggled().connect([this] {
    for (int j = 0; j < 8; j++)
    {
      if (this->available_mask & (1 << j))
      {
        this->camera_ptr_array[j]->setExposureOffsetFlag(this->evref_check.get_active());
      }
    }
  });

  evref_scale.signal_value_changed().connect([this] {
    for (int j = 0; j < 8; j++)
    {
      if (this->available_mask & (1 << j))
      {
        this->camera_ptr_array[j]->setExposureOffset(this->evref_scale.get_value());
      }
    }
  });

  ae_grid.attach(evref_check, 0, 3, 3, 1);
  ae_grid.attach(evref_label, 0, 4, 1, 1);
  ae_grid.attach(evref_scale, 1, 4, 2, 1);

  AE_frame.set_label("AE mode setting");
  AE_frame.add(ae_grid);
}

void SampleWindow::createImageTuningFrame(void)
{
  scaleConfig(hue_scale, -90, 90, 0);
  scaleConfig(saturation_scale, 0.0, 1.992188, 1.0);
  scaleConfig(contrast_scale, 0.0, 1.992188, 1.0);
  scaleConfig(brightness_scale, -256.0, 255.75, 0);
  scaleConfig(sharpness_scale, 0.0, 3.984375, 1.0);

  digital_gain_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::digitalgain_callback));
  brightness_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::brightness_callback_scale));
  hue_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::hue_callback_scale));
  saturation_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::saturation_callback_scale));
  contrast_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::contrast_callback_scale));
  sharpness_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::sharpness_callback_scale));

  image_tune_frame.set_label("Image tuning");
  image_tune_frame.add(m_grid);

  m_grid.set_column_homogeneous(true);

  int row = 0;

  m_grid.attach(hue_scale_label, 0, row, 1, 1);
  m_grid.attach(hue_scale, 1, row, 4, 1);
  row += 1;
  m_grid.attach(saturation_scale_label, 0, row, 1, 1);
  m_grid.attach(saturation_scale, 1, row, 4, 1);
  row += 1;
  m_grid.attach(contrast_scale_label, 0, row, 1, 1);
  m_grid.attach(contrast_scale, 1, row, 4, 1);
  row += 1;
  m_grid.attach(brightness_scale_label, 0, row, 1, 1);
  m_grid.attach(brightness_scale, 1, row, 4, 1);

  row += 1;
  m_grid.attach(sharpness_scale_label, 0, row, 1, 1);
  m_grid.attach(sharpness_scale, 1, row, 4, 1);
}

void SampleWindow::createControlFrame(void)
{
  save_button.set_sensitive(false);
  h_control_box.pack_end(save_button);
  h_control_box.pack_start(default_button);
  h_control_box.pack_start(load_button);

  save_button.set_label("save");
  load_button.set_label("load");
  default_button.set_label("default");
  save_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindow::save_callback));
  default_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindow::callback_default_button));
  load_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindow::callback_load_button));
}

////

void SampleWindow::load_all_value()
{
  hue_scale.set_value(camera_ptr_array[0]->getHue());
  saturation_scale.set_value(camera_ptr_array[0]->getSaturation());
  contrast_scale.set_value(camera_ptr_array[0]->getContrast());
  brightness_scale.set_value(camera_ptr_array[0]->getBrightness());
  sharpness_scale.set_value(camera_ptr_array[0]->getSharpness());
}

SampleWindow::SampleWindow(int width, int height)
  : title_label("select_port")
  , hue_scale_label("Hue")
  , saturation_scale_label("Saturation")
  , contrast_scale_label("Contrast")
  , brightness_scale_label("Brightness")
  , sharpness_scale_label("Sharpness")
  , digital_gain_label("Digital gain")
{
  ///
  for (int i = 0; i < 8; i++)
  {
    camera_ptr_array[i] = std::make_shared<C1>(i);
  }
  ///
  ///
  //

  available_mask = getAvailableCamera();

  set_title("t4cam_ctrl");
  set_default_size(width, height);
  set_border_width(16);

  add(v_box);
  // v_box.pack_start(title_label);
  //
  //
  createSelectPortFrame();
  createAEFrame();
  createImageTuningFrame();
  createControlFrame();

  v_box.pack_start(select_port_frame);
  v_box.pack_start(AE_frame);
  v_box.pack_start(spacer[0]);
  v_box.pack_start(image_tune_frame);
  v_box.pack_start(spacer[1]);

  v_box.pack_end(h_control_box);

  load_all_value();

  show_all_children();
  //  v_box.show();
}

uint16_t SampleWindow::getAvailableCamera()
{
  uint16_t ret = 0;

  for (int i = 0; i < 7; i++)
  {
    bool is_available = camera_ptr_array[i]->isAvailableCamera();
    if (is_available)
    {
      ret |= 1 << i;
    }
  }

  return ret;
}
