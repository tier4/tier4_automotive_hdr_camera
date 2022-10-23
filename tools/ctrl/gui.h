#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <gtkmm.h>

#define DEBUG
#include <t4cam_tools.hpp>

// スペーサ
class Spacer : public Gtk::Widget
{
public:
  Spacer()
  {
    set_has_window(false);
  }
};

class SampleWindow : public Gtk::Window
{
public:
  SampleWindow(int width, int height);
  virtual ~SampleWindow() = default;

private:
  // non gui
  std::array<std::shared_ptr<C1>, 8> camera_ptr_array;
  uint16_t getAvailableCamera();
  std::array<Gtk::CheckButton, 8> buttons;

  // GUI
  // Frame
  // Select Port
  void createSelectPortFrame(void);
  Gtk::Frame select_port_frame;

  //
  void createAEFrame(void);

  //
  void createImageTuningFrame(void);

  void createControlFrame(void);

  Gtk::Scale hue_scale;
  Gtk::Scale saturation_scale;
  Gtk::Scale contrast_scale;
  Gtk::Scale brightness_scale;
  Gtk::Scale sharpness_scale;

  Gtk::Scale digital_gain_scale;
  Gtk::Scale shutter_speed_scale;
  // Gtk::Scale shutter_min_scale;
  // Gtk::Scale shutter_max_scale;

  Gtk::Grid m_grid;
  void brightness_callback_scale();
  void hue_callback_scale();
  void saturation_callback_scale();
  void contrast_callback_scale();
  void sharpness_callback_scale();
  void save_callback();
  void digitalgain_callback();

  void callback_check(int i);
  void callback_radio(int i);
  void callback_default_button();
  void callback_load_button();

  void load_all_value();

  Spacer spacer[5];

  uint16_t available_mask = 0;

  Gtk::VBox v_box;
  Gtk::HBox h_check_box;
  Gtk::HBox h_control_box;
  Gtk::HBox ae_radio_box;

  Gtk::Grid ae_grid;
  Gtk::Label digital_gain_label;
  Gtk::Label shutter_speed_label;

  Gtk::Label evref_label;
  Gtk::CheckButton evref_check;
  Gtk::Scale evref_scale;

  Gtk::Frame AE_frame;
  Gtk::Frame image_tune_frame;

  Gtk::RadioButton ae_radio[4];

  Gtk::CheckButton ae_toggle;

  Gtk::Label title_label;

  Gtk::Label hue_scale_label;
  Gtk::Label saturation_scale_label;
  Gtk::Label contrast_scale_label;
  Gtk::Label brightness_scale_label;
  Gtk::Label sharpness_scale_label;
  Gtk::Button save_button;

  // control frame
  Gtk::Frame control_frame;
  Gtk::Button load_button;
  Gtk::Button default_button;
};

/*
 * create Freme
 */

#endif
