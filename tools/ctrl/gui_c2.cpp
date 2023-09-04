#include "Slider.h"
#include "cmdline.h"
#include "gui.h"

// Frame
//
void SampleWindowC2::createSelectPortFrame(void)
{
  // port Frame

  for (int i = 0; i < 8; i++)
  {
    buttons[i].set_label(std::to_string(i));
    h_check_box.pack_start(buttons[i]);
    buttons[i].signal_toggled().connect([this, i] { this->callback_check(i); });
 
    buttons[i].set_active(i == 0 && (i & master_available_mask));

    if (!(master_available_mask & (1 << i)))
    {
      buttons[i].set_sensitive(false);
    }
  }
  select_port_frame.set_label("select ports");
  select_port_frame.add(h_check_box);
}

void SampleWindowC2::createAEFrame(void)
{
  // get digital gain
  scaleConfig(sensor_gain_scale, 1, 503, 1,1);
  sensor_gain_scale.set_sensitive(false);
  sensor_gain_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<float>(&C2::setSensorGain, (float)value);
      });

  sensor_gain_label.set_label("sensor gain");


  scaleConfig(isp_sensor_gain_scale, 1, 31.99, 1, 1);
  isp_sensor_gain_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<float>(&C2::setISPSensorGain, (float)value);
      });
  isp_sensor_gain_label.set_label("isp_sensor gain");



  isp_sensor_gain_scale.set_sensitive(false);

  ae_radio[0].set_label("AE(Auto)");
  ae_radio[3].set_label("Manual");

  ae_radio[0].set_active(true);

  Gtk::RadioButton::Group ae_radio_group = ae_radio[0].get_group();
  ae_radio[3].set_group(ae_radio_group);

  ae_radio_box.pack_start(ae_radio[0]);
  ae_radio[0].signal_toggled().connect([this] { this->callback_radio(0); });
  ae_radio_box.pack_start(ae_radio[3]);
  ae_radio[3].signal_toggled().connect([this] { this->callback_radio(3); });

  scaleConfig(shutter_speed_scale, 0, 51.0, 11, 0);
  shutter_speed_scale.set_sensitive(true);
  shutter_speed_label.set_label("shutter_speed(AE)");

  shutter_speed_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<uint16_t>(&C2::setShutterTimeOnAE, (uint16_t)value);
        }
      );

  ae_grid.set_column_homogeneous(true);
  ae_grid.attach(ae_radio_box, 0, 0, 3, 1);
  ae_grid.attach(sensor_gain_label, 0, 1, 1, 1);
  ae_grid.attach(sensor_gain_scale, 1, 1, 2, 1);
  ae_grid.attach(isp_sensor_gain_label, 0, 2, 1, 1);
  ae_grid.attach(isp_sensor_gain_scale, 1, 2, 2, 1);
  ae_grid.attach(shutter_speed_label, 0, 3, 1, 1);
  ae_grid.attach(shutter_speed_scale, 1, 3, 2, 1);

  AE_frame.set_label("AE mode setting");
  AE_frame.add(ae_grid);
}
void SampleWindowC2::createAWBFrame(void)
{
  awb_radio[0].set_label("AWB on");
  awb_radio[1].set_label("Manual");

  awb_radio[0].set_active(true);

  Gtk::RadioButton::Group awb_radio_group = awb_radio[0].get_group();
  awb_radio[1].set_group(awb_radio_group);

  awb_radio_box.pack_start(awb_radio[0]);
  awb_radio[0].signal_toggled().connect(
      [this]
      {
        if (awb_radio[0].get_active())
        {
          awb_r_scale.set_sensitive(false);
          awb_g_scale.set_sensitive(false);
          awb_b_scale.set_sensitive(false);

          exec_available_cam<bool>(&C2::setAutoWhiteBalance, true);
        }
      });
  awb_radio_box.pack_start(awb_radio[1]);
  awb_radio[1].signal_toggled().connect(
      [this]
      {
        if (awb_radio[1].get_active())
        {
          awb_r_scale.set_sensitive(true);
          awb_g_scale.set_sensitive(true);
          awb_b_scale.set_sensitive(true);
          exec_available_cam<bool>(&C2::setAutoWhiteBalance,false);
        }
      });

  awb_grid.set_column_homogeneous(true);
  awb_grid.attach(awb_radio_box, 0, 0, 4, 1);

  awb_r_label.set_label("r gain");
  awb_g_label.set_label("g gain");
  awb_b_label.set_label("b gain");

  awb_r_scale.set_sensitive(false);
  scaleConfig(awb_r_scale, 0, 500, 0,0);
   awb_g_scale.set_sensitive(false);
  scaleConfig(awb_g_scale, 0, 500, 0,0);
  awb_b_scale.set_sensitive(false);
  scaleConfig(awb_b_scale, 0, 500, 0,0);


  awb_r_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<int>(&C2::setAutoWhiteBalanceGainR,(int)value);
      });

#if 1
  awb_g_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<int>(&C2::setAutoWhiteBalanceGainG,(int)value);
      });
#endif

  awb_b_scale.set_callbackvaluechanged(
      [this](double value)
      {
        exec_available_cam<int>(&C2::setAutoWhiteBalanceGainB,(int)value);
      });
  awb_grid.attach(awb_r_label, 0, 1, 1, 1);
  awb_grid.attach(awb_r_scale, 1, 1, 2, 1);
//  awb_grid.attach(awb_g_label, 0, 2, 1, 1);
//  awb_grid.attach(awb_g_scale, 1, 2, 2, 1);
  awb_grid.attach(awb_b_label, 0, 2, 1, 1);
  awb_grid.attach(awb_b_scale, 1, 2, 2, 1);

  AWB_frame.set_label("AWB mode setting");
  AWB_frame.add(awb_grid);
}

void SampleWindowC2::timer_function(void){
  static bool init = true;

  while(saturation_en_button.get_active()){
    if(saturation_en_button.get_active()){
      if(prev_saturation_val != saturation_val || init){
        prev_saturation_val = saturation_val;
        init = false;
	exec_available_cam<bool>(&C2::setSaturation, false);
        exec_available_cam<int>(&C2::setSaturationVal, int(saturation_scale.get_value()));
        //exec_available_cam<bool>(&C2::setSaturation, true);
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  exec_available_cam<bool>(&C2::setSaturation, true);
  init = true;

}

void SampleWindowC2::createImageTuningFrame(void)
{
  hue_en_button.set_active(false);
  hue_en_button.signal_toggled().connect([this] {
      exec_available_cam<bool>(&C2::setHue, hue_en_button.get_active());
		  hue_scale.set_sensitive(hue_en_button.get_active()); });
  saturation_en_button.set_active(false);
  saturation_en_button.signal_toggled().connect(
      [this]
      {
      if(saturation_en_button.get_active() == false){
      saturation_scale.set_sensitive(saturation_en_button.get_active());
        if(timer_thread.joinable()){
          timer_thread.join();
          fprintf(stderr,"joined!!");
        }
      }else{
        timer_thread = std::thread(&SampleWindowC2::timer_function, this);
        saturation_scale.set_sensitive(saturation_en_button.get_active());
        } 
             
      });
  contrast_en_button.set_active(false);
  contrast_en_button.signal_toggled().connect(
      [this]
      {
      exec_available_cam<bool>(&C2::setContrast, contrast_en_button.get_active());
        contrast_scale.set_sensitive(contrast_en_button.get_active());
      });
  brightness_en_button.set_active(false);
  brightness_en_button.signal_toggled().connect(
      [this]
      {
      exec_available_cam<bool>(&C2::setBrightness, brightness_en_button.get_active());

        brightness_scale.set_sensitive(brightness_en_button.get_active());
      });
  sharpness_en_button.set_active(false);
  sharpness_en_button.signal_toggled().connect(
      [this]
      {
      exec_available_cam<bool>(&C2::setSharpness, sharpness_en_button.get_active());
        sharpness_scale.set_sensitive(sharpness_en_button.get_active());
      });

  hue_scale.set_sensitive(false);
  scaleConfig(hue_scale, -180, 180, 0, 0);
  saturation_scale.set_sensitive(false);
  scaleConfig(saturation_scale, 0, 255, 1, 0);
  contrast_scale.set_sensitive(false);
  scaleConfig(contrast_scale, -255, 255, 0, 0);
  brightness_scale.set_sensitive(false);
  scaleConfig(brightness_scale, -255, 255, 0, 0);
  sharpness_scale.set_sensitive(false);
  scaleConfig(sharpness_scale, 0, 255, 1,  0);

  brightness_scale.set_callbackvaluechanged(
      std::bind(&SampleWindowC2::brightness_callback_scale, std::ref(*this), std::placeholders::_1));
  hue_scale.set_callbackvaluechanged(
      std::bind(&SampleWindowC2::hue_callback_scale, std::ref(*this), std::placeholders::_1));
  saturation_scale.set_callbackvaluechanged(
      std::bind(&SampleWindowC2::saturation_callback_scale, std::ref(*this), std::placeholders::_1));
  contrast_scale.set_callbackvaluechanged(
      std::bind(&SampleWindowC2::contrast_callback_scale, std::ref(*this), std::placeholders::_1));
  sharpness_scale.set_callbackvaluechanged(
      std::bind(&SampleWindowC2::sharpness_callback_scale, std::ref(*this), std::placeholders::_1));

  image_tune_frame.set_label("Image tuning");
  image_tune_frame.add(m_grid);

  m_grid.set_column_homogeneous(true);

  int row = 0;
  // grid.attach(widget, left, top, higth, width);
  m_grid.attach(hue_scale_label, 0, row, 1, 1);
  m_grid.attach(hue_en_button, 2, row, 1, 1);
  m_grid.attach(hue_scale, 3, row, 8, 1);
  row += 1;
  m_grid.attach(saturation_scale_label, 0, row, 1, 1);
  m_grid.attach(saturation_en_button, 2, row, 1, 1);
  m_grid.attach(saturation_scale, 3, row, 8, 1);
  row += 1;
  m_grid.attach(contrast_scale_label, 0, row, 1, 1);
  m_grid.attach(contrast_en_button, 2, row, 1, 1);
  m_grid.attach(contrast_scale, 3, row, 8, 1);
  row += 1;
  m_grid.attach(brightness_scale_label, 0, row, 1, 1);
  m_grid.attach(brightness_en_button, 2, row, 1, 1);
  m_grid.attach(brightness_scale, 3, row, 8, 1);

  row += 1;
  m_grid.attach(sharpness_scale_label, 0, row, 1, 1);
  m_grid.attach(sharpness_en_button, 2, row, 1, 1);
  m_grid.attach(sharpness_scale, 3, row, 8, 1);
}

void SampleWindowC2::createControlFrame(void)
{
#if 0
  h_control_box.pack_end(save_button);
  h_control_box.pack_start(default_button);
  h_control_box.pack_start(load_button);

  save_button.set_label("save");
  load_button.set_label("load");
  default_button.set_label("default");
  save_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindowC2::save_callback));
  default_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindowC2::callback_default_button));
  load_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindowC2::callback_load_button));
#endif
}

////

void SampleWindowC2::load_all_value()
{
}

void SampleWindowC2::CreateWindow(int width, int height, bool _debug_print)
{
  std::string title_name("t4cam_ctrl: for C2 Camera");
  std::string version_name("Preview release");
  std::string title = title_name + " : " + version_name;
  ///
  for (int i = 0; i < 8; i++)
  {
    camera_ptr_array[i] = std::make_shared<C2>(i);
  }

  master_available_mask = getAvailableCamera();

  set_title(title);
  set_default_size(width, height);

#if 0
  // アクショングループの作成とアクション登録
  m_actiongroup = Gtk::ActionGroup::create();
  m_actiongroup->add(Gtk::Action::create("OpenMenu", Gtk::Stock::OPEN),
                     sigc::mem_fun(*this, &SampleWindowC2::callback_load_button));
  m_actiongroup->add(Gtk::Action::create("InitMenu", "Parameter initialize"),
                     sigc::mem_fun(*this, &SampleWindowC2::callback_default_button));
  m_actiongroup->add(Gtk::Action::create("SaveMenu", Gtk::Stock::SAVE_AS), Gtk::AccelKey('s', Gdk::CONTROL_MASK),
                     sigc::mem_fun(*this, &SampleWindowC2::save_callback));
  m_actiongroup->add(Gtk::Action::create("QuitMenu", Gtk::Stock::QUIT),
                     sigc::mem_fun(*this, &SampleWindowC2::callback_quit));

  menu = Gtk::manage(new Gtk::MenuBar());
  Gtk::MenuItem *menuitem = Gtk::manage(new Gtk::MenuItem("File"));
  menu->append(*menuitem);
  Gtk::Menu *submenu = Gtk::manage(new Gtk::Menu());
  menuitem->set_submenu(*submenu);

  Glib::RefPtr<Gtk::Action> action;
  (action = m_actiongroup->get_action("InitMenu"))->set_accel_group(get_accel_group());
  submenu->append(*Gtk::manage(action->create_menu_item()));

  (action = m_actiongroup->get_action("OpenMenu"))->set_accel_group(get_accel_group());
  submenu->append(*Gtk::manage(action->create_menu_item()));

  (action = m_actiongroup->get_action("SaveMenu"))->set_accel_group(get_accel_group());
  submenu->append(*Gtk::manage(action->create_menu_item()));

  (action = m_actiongroup->get_action("QuitMenu"))->set_accel_group(get_accel_group());
  submenu->append(*Gtk::manage(action->create_menu_item()));

  v_box.pack_start(*menu, Gtk::PACK_SHRINK, 0);
#endif
  add(v_box);
  
  createSelectPortFrame();
  createAEFrame();
  createAWBFrame();
  createImageTuningFrame();
  createControlFrame();

  v_box_content.set_border_width(10);

  v_box_content.pack_start(select_port_frame, Gtk::PACK_EXPAND_WIDGET, 5);
  v_box_content.pack_start(AE_frame, Gtk::PACK_EXPAND_WIDGET, 5);
  v_box_content.pack_start(AWB_frame, Gtk::PACK_EXPAND_WIDGET, 5);
  v_box_content.pack_start(image_tune_frame, Gtk::PACK_EXPAND_WIDGET, 5);
  v_box_content.pack_end(h_control_box, Gtk::PACK_EXPAND_WIDGET, 5);
  v_box.pack_start(v_box_content);
  load_all_value();
}

SampleWindowC2::SampleWindowC2(Glib::RefPtr<Gtk::Application> _app, int width, int height, bool _debug_print)
  : debug_print(_debug_print)
  , title_label("select_port")
  , hue_scale_label("Hue")
  , saturation_scale_label("Saturation")
  , contrast_scale_label("Contrast")
  , brightness_scale_label("Brightness")
  , sharpness_scale_label("Sharpness")
{
  app = _app;

  CreateWindow(width, height, _debug_print);

  debug_printf("show display\n");

  show_all_children();
  //  v_box.show();
}

uint16_t SampleWindowC2::getAvailableCamera()
{
  uint16_t ret = 0;
  for (int i = 0; i < 7; i++)
  {
    if (camera_ptr_array[i] && camera_ptr_array[i]->isAvailableCamera())
    {
      ret |= 1 << i;
    }
  }
  if(ret == 0){
    Gtk::MessageDialog diag("C2 camera connection not found.\nPlease reconnect the C2 camera and then restart the system.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    diag.run();
  }

  return ret;
}
