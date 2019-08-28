#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define PIN_V_BATALL        A0
#define PIN_V_BAT1          A1
#define PIN_V_AMPERMETR     A2
#define PIN_BTN_FIER        2
#define PIN_FIER            3
#define PIN_AMPERMETR       4
#define PIN_BTN_DOWN        5
#define PIN_BTN_UP          6
#define PIN_SING_CHARGER    7

#define V_BATALL_ADJ 9.880/1024.0
#define V_BAT1_ADJ   4.954/1024.0
#define V_AMPER_ADJ  4.954/1024.0
#define AMPER_CORR   2.35

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
float val_bat_all = 0, val_bat_1 = 0, val_bat_2 = 0;
float val_fier_volt = 0, val_ampermemt = 0;
bool pin_fier_state = 0, pin_btnup_state = 0, pin_btndown_state = 0;
byte val = 125;
const float R_coil = 6;
unsigned long time_draw = millis();
#include "screen.h"
//#include "helper.h"


void setup() {
  Serial.begin(115200);
  pinMode(PIN_SING_CHARGER, INPUT);
  pinMode(PIN_V_BATALL, INPUT);
  pinMode(PIN_V_BAT1, INPUT);
  pinMode(PIN_BTN_FIER, INPUT_PULLUP);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);

  //  pinMode(PIN_V_AMPERMETR, INPUT);
  //  pinMode(PIN_AMPERMETR, OUTPUT);
  //  digitalWrite(PIN_AMPERMETR, HIGH);

  pinMode(PIN_FIER, OUTPUT);
  digitalWrite(PIN_FIER, LOW);
  u8g2.begin();
  u8g2_void();
}

void loop() {
  val_bat_all = V_BATALL_ADJ * analogRead(PIN_V_BATALL);
  val_bat_1   = V_BAT1_ADJ * analogRead(PIN_V_BAT1);
  val_bat_2   = val_bat_all - val_bat_1;

  pin_fier_state = !digitalRead(PIN_BTN_FIER);
  pin_btnup_state = !digitalRead(PIN_BTN_UP);
  pin_btndown_state = !digitalRead(PIN_BTN_DOWN);

  if ((millis() - time_draw) > 1000)
  {
    time_draw = millis ();
    Serial.println (time_draw);

    u8g2.clearBuffer();
    draw_main_mask("Volume");

    if (pin_fier_state)  {
      analogWrite(PIN_FIER, val);
      val_ampermemt = (val_fier_volt - val_bat_all) / R_coil;

      Serial.print(val_fier_volt);
      Serial.print(" - ");
      Serial.print(val_bat_all);
      Serial.print("=");
      Serial.print(val_fier_volt - val_bat_all);
      Serial.print(" / ");
      Serial.print(R_coil);
      Serial.print(" = ");
      Serial.println(val_ampermemt);
      draw_amper_val (50, 50, val_ampermemt);
    }  else  {
      digitalWrite(PIN_FIER, LOW);
      val_fier_volt = val_bat_all;
    }

    if (pin_btnup_state && val < 255) {
      Serial.print("up");
      val++;
    }
    if (pin_btndown_state && val > 0) {
      Serial.print("down");
      val--;
    }
    draw_all_bat_voltage (0, 0, val_bat_all);
    draw_fier_power (35, 16, int(val / 2.55));

    if (digitalRead(PIN_SING_CHARGER))
    {
      draw_charge_state(2, 9);
    }
    else
    {
      draw_batt_icon(0, 9, val_bat_1);
      draw_batt_icon(0, 20, val_bat_2);
    }
    //  u8g2.setCursor(0, 7);
    //  u8g2.print(String(val_bat_all));
    //  u8g2.setCursor(25, 7);
    //  u8g2.print(String(analogRead(PIN_V_BATALL)));
    //  u8g2.setCursor(0, 15);
    //  u8g2.print(String(val_bat_1));
    //  u8g2.setCursor(25, 15);
    //  u8g2.print(String(analogRead(PIN_V_BAT1)));
    //  u8g2.setCursor(0, 23);
    //  u8g2.print(String(val_bat_2));
    //  u8g2.setCursor(0, 31 );
    //  u8g2.print(String(val_ampermemt));
    //  u8g2.setCursor(25, 31);
    //  u8g2.print(String(analogRead(PIN_V_AMPERMETR)));
    //  u8g2.setCursor(0, 39);
    //  String powmess = String(val / 2.55);
    //  u8g2.print(powmess);
    u8g2.sendBuffer();
  }
}
