#include "Slider.h"
#include <gtkmm.h>
#include <iomanip>

Slider::Slider()
{
  initialize();
}

void Slider::initialize()
{
  slider.signal_value_changed().connect(sigc::mem_fun(*this, &Slider::callback_slidervaluechange));
  //slider.signal_adjust_bounds().connect(sigc::mem_fun(*this, &Slider::callback_slidervaluechanged));
  slider.set_draw_value(false);
  slider.show();
  label.override_color(Gdk::RGBA("gray58"), Gtk::STATE_FLAG_NORMAL);
  label.show();

  set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  pack_start(slider, Gtk::PACK_EXPAND_WIDGET);
  pack_start(label, Gtk::PACK_SHRINK);

  show();
}

void Slider::callback_slidervaluechanged(double v){
  fprintf(stderr,"hogehoge\n");
  
}

void Slider::callback_slidervaluechange()
{
  auto v = slider.get_value();

  // Convert the number to a formatted string with the specified decimal places
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(digitsNum) << v;
  std::string formattedNumber = oss.str();

  label.set_text(formattedNumber);

  if (usercallback_slidervaluechanged != nullptr)
  {
    usercallback_slidervaluechanged(v);
  }
}

void Slider::set_range(double min, double max)
{
  slider.set_range(min, max);
}

void Slider::set_value(double value)
{
  callback_slidervaluechange();
  slider.set_value(value);
}

void Slider::set_digits(int digit)
{
  digitsNum = digit;
  callback_slidervaluechange();
}

void Slider::set_increments(double va, double vb)
{
  slider.set_increments(va, vb);
}

void Slider::set_label_chars(int chars)
{
  label.set_width_chars(chars);
}

void Slider::set_text(std::string text)
{
  label.set_text(text);
}

void Slider::set_sensitive(bool sensitive)
{
  slider.set_sensitive(sensitive);
}

double Slider::get_value()
{
  return slider.get_value();
}

void Slider::set_callbackvaluechanged(Slider::CallbackValueChanged cb)
{
  usercallback_slidervaluechanged = cb;
}
