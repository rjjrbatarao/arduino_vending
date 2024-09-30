#ifndef __RELAY_H_
#define __RELAY_H_


class Relay {
  public:
    Relay(uint8_t pin1, uint8_t pin2, uint8_t coin) {
      _pin1 = pin1;
      _pin2 = pin2;
      _coin = coin;
      pinMode(_pin1, OUTPUT);
      pinMode(_pin2, OUTPUT);
      pinMode(_coin, OUTPUT);
    }
    ~Relay() {

    }
    void begin(){
      digitalWrite(_pin1, HIGH);
      digitalWrite(_pin2, HIGH);
      digitalWrite(_coin, LOW);
    }
    
    enableRelay1() {
      digitalWrite(_pin1, LOW); // LOW
    }
    disableRelay1() {
      digitalWrite(_pin1, HIGH); // HIGH
    }
    enableRelay2() {
      digitalWrite(_pin2, LOW); // LOW
    }
    disableRelay2() {
      digitalWrite(_pin2, HIGH); // HIGH
    }
    enableRelayCoin() {
      digitalWrite(_coin, HIGH); // HIGH
    }
    disableRelayCoin() {
      digitalWrite(_coin, LOW); // LOW
    }
  private:
    uint8_t _pin1;
    uint8_t _pin2;
    uint8_t _coin;
};


#endif
