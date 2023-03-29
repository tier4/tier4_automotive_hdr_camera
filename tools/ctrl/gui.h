#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <gtkmm.h>

#include "Slider.h"
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
  SampleWindow(Glib::RefPtr<Gtk::Application> _app, int width, int height, bool _debug_print = false);
  virtual ~SampleWindow() = default;

private:
  Glib::RefPtr<Gtk::Application> app;

  // non gui
  std::array<std::shared_ptr<C1>, 8> camera_ptr_array;
  uint16_t getAvailableCamera();
  std::array<Gtk::CheckButton, 8> buttons;
  bool debug_print = false;

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

  Gtk::MenuBar *menu;
  Glib::RefPtr<Gtk::ActionGroup> m_actiongroup;

  Slider hue_scale;
  Slider saturation_scale;
  Slider contrast_scale;
  Slider brightness_scale;
  Slider sharpness_scale;

  Slider digital_gain_scale;
  Slider shutter_speed_scale;
  // Gtk::Scale shutter_min_scale;
  // Gtk::Scale shutter_max_scale;

  Gtk::Grid m_grid;
  void brightness_callback_scale(double v);
  void hue_callback_scale(double v);
  void saturation_callback_scale(double v);
  void contrast_callback_scale(double v);
  void sharpness_callback_scale(double v);
  void save_callback();
  void digitalgain_callback(double v);

  void callback_check(int i);
  void callback_radio(int i);
  void callback_default_button();
  void callback_load_button();

  void callback_quit();

  void load_all_value();

  Spacer spacer[5];

  uint16_t available_mask = 0;

  Gtk::VBox v_box;
  Gtk::VBox v_box_content;
  Gtk::HBox h_check_box;
  Gtk::HBox h_control_box;
  Gtk::HBox ae_radio_box;

  Gtk::Grid ae_grid;
  Gtk::Label digital_gain_label;
  Gtk::Label shutter_speed_label;

  Gtk::Label evref_label;
  Gtk::CheckButton evref_check;
  Slider evref_scale;

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

  void debug_printf(const char *format, ...)
  {
    if (debug_print)
    {
      va_list va;
      va_start(va, format);
      // int vprintf(const char *format, va_list ap);
      vprintf(format, va);
      va_end(va);
    }
  }

#if 0
  template <typename... Args>
  void debug_printf(const char *forma, Args const &... args)
  {
    // int printf(const char *format, ...);
    if (debug_print)
    {
      fprintf(stderr, forma, args...);
    }
  }
#endif
};

/*
 * create Freme
 */

#endif
