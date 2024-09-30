#ifndef __VENDO_H_
#define __VENDO_H_

#include <Arduino.h>
#include "DEFINES.h"
#include "SCREEN.h"
#include "BUTTON.h"
#include "BUZZER.h"
#include "RELAY.h"
#include "LED.h"
#include "SETTINGS.h"

#ifdef DBG
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLN(x)
#define DEBUG
#endif

enum Dispenser {
  TYPE_1,
  TYPE_2
};

enum Mode {
  MODE_1 = 1,
  MODE_2 = 2,
};

struct Globals {

  float time_coinslot_1 = 0;
  float time_coinslot_2 = 0;
  boolean type_1_active = false;
  boolean type_2_active = false;
  boolean pause = true;
  boolean refilling = false;
  unsigned long prev_time = millis();
  //boolean active = false;
};

volatile uint32_t credit = 0;
volatile boolean coin_inserted = false;

class Vendo  {


  public:
    Vendo() {
      // on create instance
      _vendo_button_add = new Button(ADD_PIN, BUTTON_PULLUP, BUTTON_CONFIG_DEBOUNCE);
      _vendo_button_minus = new Button(MINUS_PIN, BUTTON_PULLUP, BUTTON_CONFIG_DEBOUNCE);
      _vendo_button_select = new Button(SELECT_PIN, BUTTON_PULLUP, BUTTON_CONFIG_DEBOUNCE);
      _vendo_button_1 = new Button(BUTTON_PIN1, BUTTON_PULLUP, BUTTON_DISPENSE_DEBOUNCE);
      _vendo_button_2 = new Button(BUTTON_PIN2, BUTTON_PULLUP, BUTTON_DISPENSE_DEBOUNCE);
      _vendo_buzzer = new Buzzer(BUZZER_PIN, BUZZER_START, BUZZER_DURATION);
      _vendo_settings = new Settings();
      _vendo_screen = new Screen();
      _vendo_relay = new Relay(RELAY_PIN1, RELAY_PIN2, COIN_RELAY_PIN);
      _vendo_led_1 = new Led(LED_1, LED_INTERVAL);
      _vendo_led_2 = new Led(LED_2, LED_INTERVAL);
      _vendo_dispenser = TYPE_1;
    }
    ~Vendo() {
      // on destroy instance
    }

    static void onCoin() {
      if (digitalRead(COIN_SIGNAL_PIN) == LOW) {
        credit++;
        coin_inserted = true;
      }
    }

    void factoryReset() {
      _vendo_settings->factory();
      delay(300);
      _vendo_screen->print(1, " RESET SETTINGS ");
      delay(2000);
      _vendo_screen->print(2, " DONE ");
      delay(3000);
      asm volatile ("  jmp 0");
    }

    void begin() {
      _vendo_relay->begin();
      _vendo_led_1->begin();
      _vendo_led_2->begin();
      if (!_vendo_settings->begin())
        DEBUGLN("loaded with default settings");
      _vendo_screen->begin();

      DEBUGLN(_vendo_settings->settings.time_coinslot_1);
      DEBUGLN(_vendo_settings->settings.time_coinslot_2);
      DEBUGLN(_vendo_settings->settings.vendo_mode);
      _vendo_globals.time_coinslot_1 = _vendo_settings->settings.time_coinslot_1;
      _vendo_globals.time_coinslot_2 = _vendo_settings->settings.time_coinslot_2;
      _vendo_settings->settings.vendo_mode = MODE_2;
      if (_vendo_settings->settings.vendo_mode == MODE_2) {
        attachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN), onCoin, FALLING);
        _vendo_relay->enableRelayCoin();
      }
      delay(2000);
      _vendo_screen->print(1, "    Initializing    ");
      int ftry = 0;
      while (ftry++ < 255) {
        delay(10);
        if (_vendo_button_select->check() == LOW) {
            factoryReset();
        } 
      }
    }

    void loop() {
      _vendo_buzzer->loop();
      _vendo_led_1->loop();
      _vendo_led_2->loop();

      if (_vendo_settings->settings.vendo_mode == MODE_1) {
        _vendo_screen->print(0, " [MAINTENANCE] ");
      } else if (_vendo_settings->settings.vendo_mode == MODE_2) {
        _vendo_screen->print(0, " [SABON STATION] ");
      }

      if (_vendo_dispenser == TYPE_1) {
        _vendo_screen->print(1, "    Dish Washing    ");
      } else if (_vendo_dispenser == TYPE_2) {
        _vendo_screen->print(1, "   Fab Conditioner  ");
      }


      if (_vendo_globals.pause == false) {

        if (millis() - _vendo_globals.prev_time > DISPENSE_INTERVAL && _vendo_globals.refilling) {
          _vendo_globals.prev_time = millis();
          /*
             Free mode
          */
          if (_vendo_settings->settings.vendo_mode == MODE_1) {
            if (_vendo_dispenser == TYPE_1) {
              _vendo_globals.time_coinslot_1 -= 0.1;
              if (_vendo_globals.time_coinslot_1 <= 0.0) {
                _vendo_globals.time_coinslot_1 = _vendo_settings->settings.time_coinslot_1;
                _vendo_globals.type_1_active = false;
                _vendo_globals.refilling = false;
                _vendo_globals.pause = true;
                _vendo_relay->disableRelay1();
                _vendo_led_1->stop();
                DEBUGLN("stop 1");
                if (_vendo_settings->settings.vendo_mode == MODE_2) {
                  //detachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN));
                  //_vendo_relay->disableRelayCoin();
                }

              }
            } else if (_vendo_dispenser == TYPE_2) {
              _vendo_globals.time_coinslot_2 -= 0.1;
              if (_vendo_globals.time_coinslot_2 <= 0.0) {
                _vendo_globals.time_coinslot_2 = _vendo_settings->settings.time_coinslot_2;
                _vendo_globals.type_2_active = false;
                _vendo_globals.refilling = false;
                _vendo_globals.pause = true;
                _vendo_relay->disableRelay2();
                _vendo_led_2->stop();
                if (_vendo_settings->settings.vendo_mode == MODE_2) {
                  //detachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN));
                  //_vendo_relay->disableRelayCoin();
                }
                DEBUGLN("stop 2");
              }
            }
          }
          /*
             Coin mode
          */
          else if (_vendo_settings->settings.vendo_mode == MODE_2) {
            if (credit > 0) {

              if (_vendo_dispenser == TYPE_1) {
                _vendo_relay->enableRelay1();
                _vendo_globals.time_coinslot_1 -= 0.1;
                if (_vendo_globals.time_coinslot_1 <= 0.0) {
                  _vendo_globals.time_coinslot_1 = _vendo_settings->settings.time_coinslot_1;
                  credit--;
                }
              } else if (_vendo_dispenser == TYPE_2) {
                _vendo_relay->enableRelay2();
                _vendo_globals.time_coinslot_2 -= 0.1;
                if (_vendo_globals.time_coinslot_2 <= 0.0) {
                  _vendo_globals.time_coinslot_2 = _vendo_settings->settings.time_coinslot_2;
                  credit--;
                }
              }
            } else if (credit == 0 && coin_inserted == true) {
              _vendo_globals.refilling = false;
              _vendo_globals.pause = true;
              //_vendo_relay->disableRelayCoin();
              coin_inserted = false;
              //detachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN));
              if (_vendo_dispenser == TYPE_1) {
                _vendo_globals.type_1_active = false;
                _vendo_relay->disableRelay1();
                _vendo_led_1->stop();
              } else if (_vendo_dispenser == TYPE_2) {
                _vendo_globals.type_2_active = false;
                _vendo_relay->disableRelay2();
                _vendo_led_2->stop();
              }
            }
          }
          /*

          */
          if (_vendo_dispenser == TYPE_1) {
            _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_globals.time_coinslot_1) + " ");
          } else if (_vendo_dispenser == TYPE_2) {
            _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_globals.time_coinslot_2) + " ");
          }

        }

        _vendo_screen->print(3, "  REFILLING  ");
      } else if (_vendo_globals.pause == true) {
        _vendo_screen->print(3, "   PAUSED   ");
        if (_vendo_settings->settings.vendo_mode == MODE_2) {
          if (_vendo_dispenser == TYPE_1) {
            _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_globals.time_coinslot_1) + " ");
          } else if (_vendo_dispenser == TYPE_2) {
            _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_globals.time_coinslot_2) + " ");
          }
        }
      }


      if (_vendo_globals.refilling == false) {
        if (_vendo_dispenser == TYPE_1) {
          _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_settings->settings.time_coinslot_1) + " ");
        } else if (_vendo_dispenser == TYPE_2) {
          _vendo_screen->print(2, " Coins:" + String(credit) + " /" + String(_vendo_settings->settings.time_coinslot_2) + " ");
        }
      }

      if (_vendo_button_1->check() == LOW) {
        if (_vendo_globals.refilling == false) {
          _vendo_globals.type_1_active = true;
          _vendo_globals.refilling = true;
          _vendo_globals.time_coinslot_1 = _vendo_settings->settings.time_coinslot_1;
          _vendo_led_1->start();
          _vendo_dispenser = TYPE_1;
          if (_vendo_settings->settings.vendo_mode == MODE_2) {
            //attachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN), onCoin, FALLING);
            _vendo_globals.pause = true;
            _vendo_relay->enableRelayCoin();
          } else if (_vendo_settings->settings.vendo_mode == MODE_1) {
            _vendo_globals.pause = false;
            _vendo_relay->enableRelay1();
          }
        } else {
          if (_vendo_globals.type_1_active == true) {
            _vendo_globals.pause = !_vendo_globals.pause;
            if (_vendo_globals.pause == false) {
              if (_vendo_settings->settings.vendo_mode == MODE_2) {
                if (credit > 0) {
                  _vendo_relay->enableRelay1();
                }
              } else if (_vendo_settings->settings.vendo_mode == MODE_1) {
                _vendo_relay->enableRelay1();
              }
            } else {
              _vendo_relay->disableRelay1();
            }
          } else {
            if (credit == 0) {
              _vendo_globals.time_coinslot_2 = _vendo_settings->settings.time_coinslot_2;
              _vendo_globals.type_2_active = false;
              _vendo_globals.refilling = false;
              _vendo_globals.pause = true;
              _vendo_led_2->stop();
            }
          }
        }
        DEBUGLN("pressed btn 1");
      }
      if (_vendo_button_2->check() == LOW) {
        if (_vendo_globals.refilling == false) {
          _vendo_globals.type_2_active = true;
          _vendo_globals.refilling = true;
          _vendo_globals.time_coinslot_2 = _vendo_settings->settings.time_coinslot_2;
          _vendo_led_2->start();
          _vendo_dispenser = TYPE_2;
          if (_vendo_settings->settings.vendo_mode == MODE_2) {
            //attachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN), onCoin, FALLING);
            _vendo_globals.pause = true;
            _vendo_relay->enableRelayCoin();
          } else if (_vendo_settings->settings.vendo_mode == MODE_1) {
            _vendo_globals.pause = false;
            _vendo_relay->enableRelay2();
          }
        } else {
          if (_vendo_globals.type_2_active == true) {
            _vendo_globals.pause = !_vendo_globals.pause;
            if (_vendo_globals.pause == false) {
              if (_vendo_settings->settings.vendo_mode == MODE_2) {
                if (credit > 0) {
                  _vendo_relay->enableRelay2();
                }
              } else if (_vendo_settings->settings.vendo_mode == MODE_1) {
                _vendo_relay->enableRelay2();
              }
            } else {
              _vendo_relay->disableRelay2();
            }
          } else {
            if (credit == 0) {
              _vendo_globals.time_coinslot_1 = _vendo_settings->settings.time_coinslot_1;
              _vendo_globals.type_1_active = false;
              _vendo_globals.refilling = false;
              _vendo_globals.pause = true;
              _vendo_led_1->stop();
            }
          }
        }
        DEBUGLN("pressed btn 2");
      }
      if (_vendo_button_add->check() == LOW) {
        _vendo_buzzer->beep();
        if (_vendo_dispenser == TYPE_1) {
          _vendo_settings->settings.time_coinslot_1 += 0.1;
          if (_vendo_settings->settings.time_coinslot_1 >= 999)_vendo_settings->settings.time_coinslot_1 = 999;
        } else if (_vendo_dispenser == TYPE_2) {
          _vendo_settings->settings.time_coinslot_2 += 0.1;
          if (_vendo_settings->settings.time_coinslot_2 >= 999)_vendo_settings->settings.time_coinslot_2 = 999;
        }
        _vendo_settings->store();
        DEBUGLN("pressed btn add");
      }
      if (_vendo_button_minus->check() == LOW) {
        _vendo_buzzer->beep();
        if (_vendo_dispenser == TYPE_1) {
          _vendo_settings->settings.time_coinslot_1 -= 0.1;
          if (_vendo_settings->settings.time_coinslot_1 <= 0)_vendo_settings->settings.time_coinslot_1 = 0.1;
        } else if (_vendo_dispenser == TYPE_2) {
          _vendo_settings->settings.time_coinslot_2 -= 0.1;
          if (_vendo_settings->settings.time_coinslot_2 <= 0)_vendo_settings->settings.time_coinslot_2 = 0.1;
        }
        _vendo_settings->store();
        DEBUGLN("pressed btn minus");
      }
      if (_vendo_button_select->check() == LOW) {
        _vendo_buzzer->beep();
        if (_vendo_settings->settings.vendo_mode == MODE_2) {
          _vendo_settings->settings.vendo_mode = MODE_1;
          _vendo_relay->disableRelayCoin();
        } else if (_vendo_settings->settings.vendo_mode == MODE_1) {
          _vendo_settings->settings.vendo_mode = MODE_2;
          _vendo_relay->enableRelayCoin();
        }
        //_vendo_settings->store();

        DEBUGLN("pressed btn select");
      }
    }
  private:
    Settings *_vendo_settings;
    Button *_vendo_button_1;
    Button *_vendo_button_2;
    Button *_vendo_button_add;
    Button *_vendo_button_minus;
    Button *_vendo_button_select;
    Buzzer *_vendo_buzzer;
    Screen *_vendo_screen;
    Relay *_vendo_relay;
    Led *_vendo_led_1;
    Led *_vendo_led_2;
    Dispenser _vendo_dispenser;
    Globals _vendo_globals;

};






#endif
