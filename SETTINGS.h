#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <EEPROM.h>

struct settings_t {
  float time_coinslot_1 = 1.2; // in seconds
  float time_coinslot_2 = 1.2; // in seconds
  uint32_t vendo_mode = 2; // 1 = no coin just refill, 2 = will use coin
};

class Settings {
  public:
    Settings() {
      // on create
    }
    ~Settings() {
      // on destroy or delete
    }
    void store() {
      EEPROM.put(0, settings);
    }
    boolean factory(){
      settings_t factory_settings;
      EEPROM.put(0, factory_settings);
    }
    boolean load() {
      EEPROM.get(0, settings);
      /*
       * add checks settings if default or not
       */
      if(settings.vendo_mode > 999 || settings.vendo_mode == 0){
        settings_t default_settings;
        settings = default_settings;
        EEPROM.put(0, default_settings);
        return false;
      }
      return true;
    }
    boolean begin(){
      return load();
    }
    settings_t settings;
    
  private:

};


#endif
