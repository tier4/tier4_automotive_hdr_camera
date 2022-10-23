#include <t4cam_tools.hpp>

#include "gui.h"

/*
 * Callback
 */

void SampleWindow::callback_default_button(void)
{
  Gtk::MessageDialog Save("Parameter initializing", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL);
  Save.set_secondary_text("Current values will be overwritten, are you sure you want to continue?");
  int response = Save.run();

  if (response == Gtk::RESPONSE_OK)
  {
    for (int j = 0; j < 8; j++)
    {
      if (available_mask & (1 << j))
      {
        camera_ptr_array[j]->setDefaultValue();
      }
    }

    Gtk::MessageDialog res("Parameter initializing...", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    res.set_secondary_text("Completed.");
    res.run();
  }
}

void SampleWindow::callback_load_button(void)
{
  Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.set_select_multiple(false);

  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  // add filters
  // *** Fix it necessary ***
  // select folder = use pattern("*")
  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("yaml");
  filter_text->add_pattern("*.yaml");
  dialog.add_filter(filter_text);
  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  int result = dialog.run();

  switch (result)
  {
    case (Gtk::RESPONSE_OK):
    {
      // *** Fix it necessary ***
      // only single select
      // std::std::string filename = dialog.get_filename();
      // std::std::cout << "selected: " <<  filename << std::endl;
      // single or multiple select
      std::vector<std::string> filenames = dialog.get_filenames();
      for (std::string& filename : filenames)
      {
        std::cout << "File open :" << filename << std::endl;
        for (int j = 0; j < 8; j++)
        {
          if (available_mask & (1 << j))
          {
            camera_ptr_array[j]->initialized_load_value_from_file(filename);
            camera_ptr_array[j]->setLoadValue();
          }
        }
        std::cout << "set the paarameter from file" << std::endl;
        load_all_value();
      }
      break;
    }
    case (Gtk::RESPONSE_CANCEL):
    {
      std::cout << "action: cancel" << std::endl;
      break;
    }
    default:
    {
      std::cout << "action: unexpected button" << std::endl;
      break;
    }
  }
}

void SampleWindow::save_callback()
{
  Gtk::MessageDialog Save("This feature is not implemented!", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  Save.set_secondary_text("This feature is not implemented.");
  Save.run();
}

void SampleWindow::callback_radio(int i)
{
  if ((int)ae_radio[i].get_active() == 1)
  {
    for (int j = 0; j < 8; j++)
    {
      if (available_mask & (1 << j))
      {
        camera_ptr_array[j]->setAEMode(i);
      }
    }
    if (i == 3)
    {
      digital_gain_scale.set_sensitive(true);
      shutter_speed_scale.set_sensitive(true);
    }
    else
    {
      digital_gain_scale.set_sensitive(false);
      shutter_speed_scale.set_sensitive(false);
    }
  }
}

/////

void SampleWindow::digitalgain_callback()
{
  for (int j = 0; j < 8; j++)
  {
    if (available_mask & (1 << j))
    {
      camera_ptr_array[j]->setDigitalGain(digital_gain_scale.get_value());
      fprintf(stderr, "AEError: %f\n", camera_ptr_array[j]->getAEError());
    }
  }
}

void SampleWindow::callback_check(int i)
{
  printf("%d:%d\n", i, (int)buttons[i].get_active());

  if (buttons[i].get_active() == 0)
  {
    available_mask &= ~(1 << i);
  }
  else
  {
    available_mask |= 1 << i;
  }
}

// image tuning
void SampleWindow::brightness_callback_scale()
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      fprintf(stderr, "%d\n", i);
      camera_ptr_array[i]->setBrightness(brightness_scale.get_value());
    }
  }
}
void SampleWindow::hue_callback_scale()
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setHue(hue_scale.get_value());
    }
  }
}
void SampleWindow::saturation_callback_scale()
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setSaturation(saturation_scale.get_value());
    }
  }
}
void SampleWindow::contrast_callback_scale()
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setContrast(contrast_scale.get_value());
    }
  }
}

void SampleWindow::sharpness_callback_scale()
{
  for (int i = 0; i < 8; i++)
  {
    if (available_mask & (1 << i))
    {
      camera_ptr_array[i]->setSharpness(sharpness_scale.get_value());
    }
  }
}
