#ifndef _DEFINES_H_
#define _DEFINES_H_

/*
 * Enable debug, comment out to disable serial debugger
 */
//#define DBG

/*
 * Pin defines
 */
#define BUTTON_PIN1 8
#define BUTTON_PIN2 12

#define ADD_PIN 9
#define SELECT_PIN 10
#define MINUS_PIN 11

#define COIN_SIGNAL_PIN 3
#define COIN_RELAY_PIN 4
#define RELAY_PIN1 5
#define RELAY_PIN2 6
#define BUZZER_PIN 7

#define LED_1 2
#define LED_2 13

/*
 * Button options
 */
#define BUTTON_PULLUP true
#define BUTTON_CONFIG_DEBOUNCE 100
#define BUTTON_DISPENSE_DEBOUNCE 50

/*
 * Beeper
 */
#define BUZZER_START false
#define BUZZER_DURATION 100

/*
 * 
 */
#define DISPENSE_INTERVAL 50

/*
 * 
 */
#define LED_INTERVAL 300

#endif
