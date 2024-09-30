# arduino_vending
Common c++ headers for building complex vending machines
## Components
- Button class
- Led class
- Relay class
- Screen i2c display class
- Persistent Settings class
- Global defines config

## How to use
```c++
// include all components
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

// create object instances
Button* vendo_button = new Button(BUTTON_PIN1, BUTTON_PULLUP, BUTTON_DISPENSE_DEBOUNCE);
Buzzer* vendo_buzzer = new Buzzer(BUZZER_PIN, BUZZER_START, BUZZER_DURATION);
Relay* vendo_relay = new Relay(RELAY_PIN1, RELAY_PIN2, COIN_RELAY_PIN);
Settings* vendo_settings = new Settings();
Screen* vendo_screen = new Screen();
Led* vendo_led = new Led(LED_1, LED_INTERVAL);

// interrupt pin callback
static void onCoin() {
  if (digitalRead(COIN_SIGNAL_PIN) == LOW) {
    credit++;
    coin_inserted = true;
  }
}

void setup(){
Serial.begin(9600);
vendo_relay->begin();
vendo_led->begin();
if (!_vendo_settings->begin()) DEBUGLN("loaded with default settings");
vendo_screen->begin();
attachInterrupt(digitalPinToInterrupt(COIN_SIGNAL_PIN), onCoin, FALLING);
}

void loop(){
vendo_buzzer->loop();
vendo_led->loop();
  if (vendo_button->check() == LOW) {
    vendo_buzzer->beep();
    vendo_relay->enableRelayCoin();
    vendo_relay->enableRelay1();
    vendo_relay->enableRelay2();
    vendo_led->start();
    //vendo_settings->store();
  }
}

```
