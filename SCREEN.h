#ifndef __SCREEN_H_
#define __SCREEN_H_

#include <Wire.h>
#include "src/LiquidCrystal_I2C/LiquidCrystal_I2C.h"

class Screen {
  public:
    Screen() {
      // on create
      lcd = new LiquidCrystal_I2C(0x27, 20, 4);
    }
    ~Screen() {
      // on destroy or delete
    }
    void begin() {
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
};



#endif
