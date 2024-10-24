# arduino_vending
Common nonblocking c++ headers for building any complex vending machines. Ie. wifi, liquid, helmet etc vending machines.

## Features v1.0.1
- Nonblocking class components provided by single .h header
- Minimal memory footprint
- (New) State Machine class
  - nestable non-blocking state machine
- (New) Credit handler class
  - handler for total credit and denomination detection
- Has all you need for simple to complex vending machines

## Class Components
- Button class
- Led class
- Relay class
- Buzzer class
- Screen i2c display class
- Settings class (Persistent)
- State Machine
- Credit Handler

## Installation
```
Copy all of the files here alongside your .ino
modify settings_t struct to your need and settings.xxxx will be 
automatically provided globally
```

## Example Code Usage
```c++
/**
   Created by rjjrbatarao

   arduino vending code
   added State machine class
   example demostrates sub state

   Note:
   Upon testing enable the delay inside loop when enabling
   the serial prints else it will flood the serial monitor.
   For production use please disable any delay inside loop
   for best performance, timers will be handled by the library
   asynchronously
*/

#include "Vendo.h"


//using namespace Vendo;

Vendo::State* state = new Vendo::State(3);// create 3 states
Vendo::State* idle_state = new Vendo::State(2); // create 2 sub states
Vendo::Button* button = new Vendo::Button(5); // button pin at pin 5
Vendo::Credit* credit = new Vendo::Credit(13); // pin for coin acceptor
Vendo::Screen* screen = new Vendo::Screen(33, 32); // sda on pin 33, scl on pin 32
Vendo::Relay* coin_relay = new Vendo::Relay(12, HIGH); // relay pin at pin 12 ouput is logic low

/*
   The id are used to
   jump between states
*/

enum {
  IDLE_ID,
  RUNNING_ID,
  STOPPED_ID
};

enum {
  SUB_IDLE_1_ID,
  SUB_IDLE_2_ID
};

void subIdleOne() {
  //Serial.println("sub idle 1");
  idle_state->next();
}

void subIdleTwo() {
  //Serial.println("sub idle 2");
  idle_state->reset();
}

void idleState() {
  //Serial.println("idle");
  if (button->check() == LOW) {
    state->next();
  }
  idle_state->loop();
}

void runningState() {
  Serial.println("running");
  state->set(STOPPED_ID);
}

void stoppedState() {
  Serial.println("stopped");
  idle_state->reset();
}

void creditCount(uint16_t total, uint16_t denominations) {
  Serial.print("total: ");
  Serial.print(total);// total
  Serial.print(" denom: ");
  Serial.println(denominations); // coin inserted
}

/**
   Please do not change this is to avoid wrong credits
*/
void coinIsr() {
  credit->credit_interrupt();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  state->add(&idleState, IDLE_ID);
  state->add(&runningState, RUNNING_ID);
  state->add(&stoppedState, STOPPED_ID);
  idle_state->add(&subIdleOne, SUB_IDLE_1_ID);
  idle_state->add(&subIdleTwo, SUB_IDLE_2_ID);

  credit->attach(&creditCount);
  attachInterrupt(13, coinIsr, FALLING); // only FALLING is allowed
  credit->enable();
  screen->begin();
  screen->print(1, "Helmet vending");
  coin_relay->begin();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  state->loop();
  credit->loop();
  //delay(1000);

}

```
## License
`
MIT License
`
