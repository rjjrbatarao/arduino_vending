#ifndef __BUZZER_H_
#define __BUZZER_H_

class Buzzer {
  public:
    Buzzer(uint8_t pin, boolean beep_flag = false, uint16_t beep_duration = 100)  {
      _beep_flag = beep_flag;
      _beep_duration = beep_duration;
      _prevState = millis();
      _pin = pin;
      if(beep_flag){ // if flag set to true initialize beep start
        digitalWrite(_pin, HIGH);
      }
      pinMode(_pin, OUTPUT);
      // on create
    }
    ~Buzzer() {
      // on destroy or delete
    }
    void beep(){
      _prevState = millis();
      _beep_flag = true;
      digitalWrite(_pin, HIGH);
    }
    void loop() {
      if (_beep_flag) {
        if(millis() - _prevState > _beep_duration){
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



#endif
