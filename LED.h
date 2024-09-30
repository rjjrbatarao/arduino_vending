#ifndef __LED_H_
#define __LED_H_


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

    start() {
      _blink = true;
      _prev_time = millis();
    }

    stop() {
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


#endif
