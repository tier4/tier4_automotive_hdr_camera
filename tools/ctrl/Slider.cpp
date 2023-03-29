
#include "Slider.h"

#include <gtkmm.h>

Slider::Slider()
{
  initialize();
}

void Slider::initialize()
{
  slider.signal_value_changed().connect(sigc::mem_fun(*this, &Slider::callback_slidervaluechange));

  slider.set_draw_value(false);
  slider.show();
  label.override_color(Gdk::RGBA("gray58"), Gtk::STATE_FLAG_NORMAL);
  label.show();

  set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  pack_start(slider, Gtk::PACK_EXPAND_WIDGET);
  pack_start(label, Gtk::PACK_SHRINK);

  show();
}

void Slider::callback_slidervaluechange()
{
  auto v = slider.get_value();

  auto dn = digitsNum < 0 ? 1 : digitsNum;

  auto printformat = Glib::ustring::sprintf("%%.0%df", dn);

  if (digitsNum == 0)
  {
    // MEMO: digitsNumが0以下の時は整数値のみ表示
    printformat = Glib::ustring::format("%.0f");
  }
  label.set_text(Glib::ustring::sprintf(printformat, v));

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