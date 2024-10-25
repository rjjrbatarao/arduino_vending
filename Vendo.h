#ifndef __VENDING_H_
#define __VENDING_H_
/**
   Created by rjjrbatarao

   arduino vending code
   added State machine class
   example demostrates sub state

   Note:
   Upon testing enable the delay inside loop when enabling
   the serial prints else it will flood the serial monitor.
   For production use please disable any delay inside loop
   for best performance, events will be handled by the library
   asynchronously
*/

#include <Wire.h>
#include <EEPROM.h>
#include "src/LiquidCrystal_I2C/LiquidCrystal_I2C.h"


#ifdef DBG
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLN(x)
#define DEBUG
#endif

#if defined(ESP32)
#define LCD_SDA 21
#define LCD_SCL 22

#elif defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
#define LCD_SDA 4
#define LCD_SCL 5

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define LCD_SDA 20
#define LCD_SCL 21

#else
#warning "Board not tested"
#define LCD_SDA 4
#define LCD_SCL 5

#endif

namespace Vendo
{

class Button {
  public:
    Button (uint8_t pin, bool pullup = false, uint32_t debounceDelay = 50)
      : _pin(pin), _state(LOW), _lastState(LOW),
        _lastMillis(0), _debounceDelay(debounceDelay),
        _lastDebounceTime(0) {
      if (pullup == true) {
        pinMode(_pin, INPUT_PULLUP);
      } else {
        pinMode(_pin, INPUT);
      }
    }

    // Debounces the button and returns the state if it was just changed.
    bool check(bool triggerState = LOW) {
      bool reading = digitalRead(_pin);
      // Checks if the buttons has changed state
      if (reading != _lastState) {
        _lastDebounceTime = millis();
      }

      // Checks if the buttons hasn't changed state for '_debounceDelay' milliseconds.
      if ((millis() - _lastDebounceTime) > _debounceDelay) {
        // Checks if the buttons has changed state
        if (reading != _state) {
          _state = reading;
          return _state;
        } else {
          /*
             this is for long press buggy as of now
          */
          //_lastDebounceTime = millis();
          //_state = !_state;
        }
      }
      _lastState = reading;

      // If this code is reached, it returns the normal state of the button.
      if (triggerState == HIGH) {
        return LOW;
      } else {
        return HIGH;
      }
    }

  private:
    const uint8_t _pin;
    bool _state;
    bool _lastState;
    uint32_t _lastMillis;
    uint32_t _debounceDelay;
    uint32_t _lastDebounceTime;
};




class Screen {
  public:
    Screen(int SDA = LCD_SDA, int SCL = LCD_SDA) {
      // on create
      lcd = new LiquidCrystal_I2C(0x27, 20, 4);
      _SDA = SDA;
      _SCL = SCL;
    }
    ~Screen() {
      // on destroy or delete
    }
    void begin() {
      if (LCD_SDA == _SDA && LCD_SCL == _SCL) {
        Wire.begin();
      } else {
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
        Wire.begin(_SDA, _SCL);
#else
        Wire.begin();
#endif
      }
      lcd->begin(20, 4);
      lcd->backlight();
    }

    void print(int row, String text) {
      text.reserve(64);
      int offset = 0;
      offset = (20 - text.length()) / 2;
      lcd->setCursor ( offset, row );
      lcd->print(text);
    }

    void clear(int row) {
      lcd->setCursor ( 0, row ); //select the row
      lcd->print("                    "); //Clear the row
    }
  private:
    LiquidCrystal_I2C *lcd;
    int _SDA;
    int _SCL;
};

/**
   This is a minimal implementation of how to
   store and retrieve settings
   please modify the settings_t struct to hold more
   variables
*/

struct settings_t {
  float time_coinslot = 1.2; // example in seconds
  float credits_minutes = 5; // example in minutes
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
    boolean factory() {
      settings_t factory_settings;
      EEPROM.put(0, factory_settings);
      return true;
    }
    boolean load() {
      EEPROM.get(0, settings);
      /*
         add checks settings if default or not
      */
      if (settings.vendo_mode > 999 || settings.vendo_mode == 0) {
        settings_t default_settings;
        settings = default_settings;
        EEPROM.put(0, default_settings);
        return false;
      }
      return true;
    }
    boolean begin() {
      return load();
    }

    settings_t settings;// public settings

  private:

};

class Buzzer {
  public:
    Buzzer(uint8_t pin, boolean beep_flag = false, uint16_t beep_duration = 100)  {
      _beep_flag = beep_flag;
      _beep_duration = beep_duration;
      _prevState = millis();
      _pin = pin;
      if (beep_flag) { // if flag set to true initialize beep start
        digitalWrite(_pin, HIGH);
      }
      pinMode(_pin, OUTPUT);
      // on create
    }
    ~Buzzer() {
      // on destroy or delete
    }
    void beep() {
      _prevState = millis();
      _beep_flag = true;
      digitalWrite(_pin, HIGH);
    }
    void loop() {
      if (_beep_flag) {
        if (millis() - _prevState > _beep_duration) {
          _beep_flag = false;
          digitalWrite(_pin, LOW);
        }
      }
    }
  private:
    uint8_t _pin;
    boolean _beep_flag;
    uint32_t _prevState;
    uint16_t  _beep_duration;
};


class Led {
  public:
    Led(uint8_t pin1, uint16_t interval = 1000) {
      _pin1 = pin1;
      _blink = false;
      _interval = interval;
      _prev_time = millis();
      pinMode(_pin1, OUTPUT);

    }
    ~Led() {

    }

    void begin() {
      digitalWrite(_pin1, LOW);
    }

    void start() {
      _blink = true;
      _prev_time = millis();
    }

    void stop() {
      _blink = false;
      digitalWrite(_pin1, LOW);
    }

    void loop() {
      if (_blink) {
        if (millis() - _prev_time > _interval) {
          _prev_time = millis();
          _state = !_state;
          digitalWrite(_pin1, _state);
        }
      }
    }
  private:
    uint8_t _pin1;
    uint32_t _prev_time;
    uint16_t _interval;
    boolean _blink;
    boolean _state;

};

class Relay {
  public:
    Relay(uint8_t pin, boolean logic = HIGH) {
      _pin = pin;
      _logic = logic;
      pinMode(_pin, OUTPUT);
    }
    ~Relay() {
    }
    void begin() {
      digitalWrite(_pin, _logic);
    }
    void enableRelay() {
      digitalWrite(_pin, !_logic); // LOW
    }
    void disableRelay() {
      digitalWrite(_pin, _logic); // HIGH
    }


  private:
    uint8_t _pin;
    boolean _logic;
};

class State {
  public:
    State(uint8_t n, uint8_t stage = 0) {
      func_ptr funcs[n];
      _stage = stage;
      _funcs = new func_ptr(*funcs);
    }
    ~State() {
      delete _funcs;
    }
    void add(void(*func)(), uint8_t index = 0) {
      _index = index;
      _funcs[index] = func;
    }
    void next() {
      if (_index == _stage) {
        return;
      }
      _stage++;
    }
    void set(uint8_t stage) {
      _stage = stage;
    }
    int get() {
      return _stage;
    }
    void reset() {
      _stage = 0;
    }
    void loop() {
      _funcs[_stage]();
    }
  private:
    uint8_t _stage;
    uint8_t _index;
    typedef void(*func_ptr)(void);
    func_ptr *_funcs;
};

class Credit {

  public:
    Credit(uint8_t pin, uint16_t debounce = 500) {
      _counting = false;
      _coinslot_active = false;
      _denomination = 0;
      _total_credits = 0;
      _debounce = debounce;
      _pin = pin;
    }
    ~Credit() {

    }
    void attach(void(*func)(uint16_t total, uint16_t denom)) {
      _funcs = func;
    }
    void enable() {
      _coinslot_active = true;
    }
    void disable() {
      _coinslot_active = false;
      _total_credits = 0;
    }
    void credit_interrupt()
    {
      if (_counting == true && digitalRead(_pin) == LOW)
      {
        _start_time = millis();
        _denomination++;
      }
    }

    void loop() {
      read_credits();
    }

  private:
    typedef void(*func_ptr)(uint16_t total, uint16_t denom);
    func_ptr _funcs;

    volatile  boolean _counting;
    volatile  unsigned long _start_time;
    volatile  uint16_t _denomination;

    uint8_t _pin;
    uint16_t _debounce;
    boolean _coinslot_active;
    uint16_t _total_credits;

    void counted_credits() {
      if (_denomination > 0)
      {
        _total_credits += _denomination;
        _funcs(_total_credits, _denomination);
      }
    }

    void read_credits() {
      if (_coinslot_active)
      {
        if (_counting)
        {
          if (millis() - _start_time < _debounce)
          {
            return;
          }
          _counting = false;
          counted_credits();
        }
        noInterrupts();
        _denomination = 0;
        _counting = true;
        interrupts();
      }
    }


};


class Timer {
  public:
    Timer(uint16_t interval = 1000) {
      _prev_time = 0;
      _counting = false;
      _interval = interval;
    }
    void on_start(void(*func)(void)) {
      _func_start = func;
    }
    void on_end(void(*func)(void)) {
      _func_stop = func;
    }
    void on_running(void(*func)(uint16_t c_ount)) {
      _func_running = func;
    }
    void start() {
      _counting = true;
      _prev_time = millis();
      _func_start();
      _count = 0;
    }
    void stop() {
      _counting = false;
      _func_stop();
      _count = 0;
    }
    void reset() {
      _count = 0;
    }
    void loop() {
      if (_counting == true) {
        if (millis() - _prev_time > _interval) {
          _prev_time = millis();
          _func_running(_count++);
        }
      }
    }
  private:
    typedef void(*func_none_ptr)(void);
    typedef void(*func_one_ptr)(uint16_t c_ount);

    func_none_ptr _func_start;
    func_none_ptr _func_stop;
    func_one_ptr _func_running;
    unsigned long _prev_time;
    boolean _counting;
    uint16_t _count;
    int16_t _interval;

};

}
#endif
