#pragma once
#include <stdio.h>
#include <gtkmm.h>

class Slider : public Gtk::Box
{
public:
  using CallbackValueChanged = std::function<void(double)>;

  Slider();
  virtual ~Slider() = default;

  virtual void initialize();

  virtual void set_range(double min, double max);
  virtual void set_value(double value);
  virtual void set_digits(int digits);
  virtual void set_increments(double va, double vb);

  virtual void set_label_chars(int chars);

  virtual void set_text(std::string text);

  virtual void set_sensitive(bool sensitive = true);

  virtual double get_value();

  /*
   * set_callbackvaluechange(std::bind(T::func,std::ref(instance),std::placeholders::_1))
   */
  virtual void set_callbackvaluechanged(CallbackValueChanged callback);

private:
  void callback_slidervaluechange();
  void callback_slidervaluechanged(double v);


  int digitsNum = -1;

  CallbackValueChanged usercallback_slidervaluechanged;

  Gtk::Scale slider;
  Gtk::Label label;
};
