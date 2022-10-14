#include <gtkmm.h>

#define DEBUG
#include <t4cam_tools.hpp>

// スペーサ
class Spacer : public Gtk::Widget
{
  public:
    Spacer(){set_has_window(false);}
};

class SampleWindow : public Gtk::Window
{
  public:
    SampleWindow(int width, int height);
    virtual ~SampleWindow() = default;

  private:
    Gtk::Scale hue_scale;
    Gtk::Scale b_scale;
    Gtk::Scale c_scale;
    Gtk::Scale m_scale;
    
    Gtk::Scale digital_gain_scale;
    Gtk::Scale shutter_speed_scale;
   // Gtk::Scale shutter_min_scale;
   // Gtk::Scale shutter_max_scale;
    
    Gtk::Grid m_grid;
    Gtk::ComboBox m_combo;
    void m_callback_scale();
    void a_callback_scale();
    void b_callback_scale();
    void c_callback_scale();
    void save_callback();
    void digitalgain_callback();

    void callback_check(int i);
    void callback_radio(int i);

    Spacer spacer[5];
    
    uint16_t available_mask =0;

    Gtk::VBox v_box;
    Gtk::HBox h_check_box;
    Gtk::HBox h_save_box;
    Gtk::HBox ae_radio_box;

    Gtk::Grid ae_grid;
    Gtk::Label digital_gain_label;
    Gtk::Label shutter_speed_label;

    std::array<std::shared_ptr<C1>, 8> camera_ptr_array;

    Gtk::Label evref_label;
    Gtk::CheckButton evref_check;
    Gtk::Scale evref_scale;

    std::array<Gtk::CheckButton, 8> buttons;
    Gtk::Frame select_port_frame;
    Gtk::Frame AE_frame;
    Gtk::Frame image_tune_frame;

    uint16_t getAvailableCamera();

    Gtk::RadioButton ae_radio[4];


    Gtk::CheckButton ae_toggle;

    Gtk::Label title_label;
    
    Gtk::Label hue_scale_label;
    Gtk::Label b_scale_label;
    Gtk::Label c_scale_label;
    Gtk::Label m_scale_label;
    Gtk::Button save_button;
};


void scaleConfig(Gtk::Scale &scale){
  scale.set_range(0,256);
  scale.set_value(128);
  scale.set_digits(2);
  scale.set_increments(0.1,1);
  scale.set_draw_value(true);
  scale.set_value_pos(Gtk::POS_RIGHT);
  //scale.set_size_request(250);
}

void scaleConfig(Gtk::Scale &scale, float min, float max, float init){
  scale.set_range(min,max);
  scale.set_value(init);
  scale.set_digits(4);
  scale.set_increments(0.1,1);
  scale.set_draw_value(true);
  scale.set_value_pos(Gtk::POS_RIGHT);
  //scale.set_size_request(250);
}

void SampleWindow::callback_radio(int i)
{
      printf("%d:%d\n",i, (int)ae_radio[i].get_active());
      if((int)ae_radio[i].get_active() == 1){
	      for(int j=0;j<8;j++){
		      if(available_mask & (1 << j)){
			      camera_ptr_array[j]->setAEMode(i);
		      }
	      }
	      if(i == 3){
		      digital_gain_scale.set_sensitive(true);
		      shutter_speed_scale.set_sensitive(true);
	      }else{
		      digital_gain_scale.set_sensitive(false);
		      shutter_speed_scale.set_sensitive(false);
	      }
      }

}

void SampleWindow::digitalgain_callback(){
      
	for(int j=0;j<8;j++){
		if(available_mask & (1 << j)){
			camera_ptr_array[j]->setDigitalGain(digital_gain_scale.get_value());
			fprintf(stderr, "AEError: %f\n",camera_ptr_array[j]->getAEError());
		}
	}



}

void SampleWindow::callback_check(int i)
{
      printf("%d:%d\n",i, (int)buttons[i].get_active());

      if(buttons[i].get_active() == 0){
	available_mask &= ~(1 << i);
      }else{
	available_mask |= 1 << i;
      }
      fprintf(stderr,"0x%x\n", available_mask);
      
}


SampleWindow::SampleWindow(int width, int height): 
  title_label("select_port"),
  hue_scale_label("hue"),
  b_scale_label("saturation"),
  c_scale_label("contrast"),
  m_scale_label("brightness"),
  digital_gain_label("digital gain")
{

  ///
  for(int i=0; i<8;i++){
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

  fprintf(stderr,"0x%x\n", available_mask);

  for(int i=0;i<8;i++){
    buttons[i].set_label(std::to_string(i));
    buttons[i].set_active(i==0);
    h_check_box.pack_start(buttons[i]);
    buttons[i].signal_toggled().connect(
        [this, i]{this->callback_check(i);});

    if(!(available_mask & (1 << i))){
	buttons[i].set_sensitive(false);
    }
  }
  

  // port Frame
  select_port_frame.set_label("select ports");
  select_port_frame.add(h_check_box);
 // v_box.pack_start(title_label);
 
  //get digital gain
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


  for(int i=0;i<4;i++){
    ae_radio_box.pack_start(ae_radio[i]);
    ae_radio[i].signal_toggled().connect(
        [this, i]{this->callback_radio(i);});
  }
 
// evref
  
  evref_label.set_label("evref_offset");
  scaleConfig(evref_scale, -12.61, 12.7, 0 );

  scaleConfig(shutter_speed_scale, 0,  1000, 11);
  shutter_speed_scale.set_sensitive(false);
shutter_speed_label.set_label("shutter_speed_fme");

  ae_grid.set_column_homogeneous(true);
  ae_grid.attach(ae_radio_box, 0, 0, 3, 1);
  ae_grid.attach(digital_gain_label, 0, 1, 1, 1);
  ae_grid.attach(digital_gain_scale, 1, 1, 2, 1);
  ae_grid.attach(shutter_speed_label, 0, 2, 1, 1);
  ae_grid.attach(shutter_speed_scale, 1, 2, 2, 1);
 

  shutter_speed_scale.signal_value_changed().connect(
		[this]{
		for(int i=0; i<8; i++){
		if(this->available_mask & (1 << i))
		{
		this->camera_ptr_array[i]->setShutterSpeedforFME(this->shutter_speed_scale.get_value());
		}

		}
		}
		  );

  
  evref_check.set_label("evref_offset_enable");
  evref_check.set_active(false);
  evref_check.signal_toggled().connect(
		  [this]{
		  for(int j=0;j<8;j++){
		  	if(this->available_mask & (1 << j))
			{
		  	  this->camera_ptr_array[j]->setExposureOffsetFlag(this->evref_check.get_active());
			  }
			  }
			  }
		  );

  evref_scale.signal_value_changed().connect(
		  [this]{
		  for(int j=0;j<8;j++){
		  	if(this->available_mask & (1 << j))
			{
		  	  this->camera_ptr_array[j]->setExposureOffset(this->evref_scale.get_value());
			  }
			  }
			  }
		  );
		  
	

  ae_grid.attach(evref_check, 0, 3, 3, 1);
  ae_grid.attach(evref_label, 0, 4, 1, 1);
  ae_grid.attach(evref_scale, 1, 4, 2, 1);
  

  AE_frame.set_label("AE mode setting");
  AE_frame.add(ae_grid);

  scaleConfig(hue_scale, -90, 90, 0);
  scaleConfig(b_scale, 0.0, 1.992188, 1.0);
  scaleConfig(c_scale, 0.0, 1.992188, 1.0);
  scaleConfig(m_scale, -256.0, 255.75, 0 );



  digital_gain_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::digitalgain_callback));
  m_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::m_callback_scale));
  hue_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::a_callback_scale));
  b_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::b_callback_scale));
  c_scale.signal_value_changed().connect(sigc::mem_fun(*this, &SampleWindow::c_callback_scale));

  image_tune_frame.set_label("Image tuning");
  image_tune_frame.add(m_grid);

  m_grid.set_column_homogeneous(true);

  int  row = 0;

  m_grid.attach(hue_scale_label, 0,row,1,1);
  m_grid.attach(hue_scale, 1,row,4,1);
  row += 1;
  m_grid.attach(b_scale_label, 0,row,1,1);
  m_grid.attach(b_scale, 1,row,4,1);
  row+=1;
  m_grid.attach(c_scale_label, 0,row,1,1);
  m_grid.attach(c_scale, 1,row,4,1);
  row +=1;
  m_grid.attach(m_scale_label, 0,row,1,1);
  m_grid.attach(m_scale, 1,row,4,1);
  
  v_box.pack_start(select_port_frame);
  v_box.pack_start(AE_frame);
  v_box.pack_start(spacer[0]);
  v_box.pack_start(image_tune_frame);
  v_box.pack_start(spacer[1]);

  save_button.set_sensitive(false);
  h_save_box.pack_end(save_button);

  save_button.set_label("save");
  save_button.signal_clicked().connect(sigc::mem_fun(*this, &SampleWindow::save_callback));
  
  v_box.pack_end(h_save_box);

  show_all_children();
//  v_box.show();
}


uint16_t SampleWindow::getAvailableCamera(){
	uint16_t ret = 0;

	for(int i=0;i<7;i++){
		bool is_available = camera_ptr_array[i]->isAvailableCamera();
		if(is_available){
		ret |= 1 << i;
		}
	}

	return ret;
}

void SampleWindow::save_callback()
{
  Gtk::MessageDialog Save("This feature is not implemented!", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
  Save.set_secondary_text("This feature is not implemented.");
  Save.run();

  

}

void SampleWindow::m_callback_scale()
{
  for(int i=0;i<8;i++){
	  if(available_mask & (1 << i)){
		  fprintf(stderr,"%d\n", i);
		  camera_ptr_array[i]->setBrightness(m_scale.get_value());
	  }
  }
}
void SampleWindow::a_callback_scale()
{
  for(int i=0;i<8;i++){
	  if(available_mask & (1 << i)){
		  camera_ptr_array[i]->setHue(hue_scale.get_value());
	  }
  }

}
void SampleWindow::b_callback_scale()
{
  for(int i=0;i<8;i++){
	  if(available_mask & (1 << i)){
		  camera_ptr_array[i]->setSaturation(b_scale.get_value());
	  }
  }
}
void SampleWindow::c_callback_scale()
{
  for(int i=0;i<8;i++){
	  if(available_mask & (1 << i)){
		  camera_ptr_array[i]->setContrast(c_scale.get_value());
	  }
  }
}


int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv);
  SampleWindow window(640,480);
  return app->run(window);
}
