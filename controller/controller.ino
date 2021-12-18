#include "LiquidCrystal_PCF8574.h"
#include "Wire.h"
#include "Seeed_HM330X.h"
#include "Seeed_SHT35.h"

#ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SDAPIN  20
    #define SCLPIN  21
    #define RSTPIN  7
    #define SERIAL SerialUSB
#else
    #define SDAPIN  A4
    #define SCLPIN  A5
    #define RSTPIN  2
    #define SERIAL Serial
#endif

uint8_t buf[30];

int getPM2_5(uint8_t* data) {
  if (data == NULL) {
    return HM330XErrorCode::ERROR_PARAM;
  }
  return data[3 * 2] << 8 | data[3 * 2 + 1];
}

int getPM10(uint8_t* data) {
  if (data == NULL) {
    return HM330XErrorCode::ERROR_PARAM;
  }
  return data[4 * 2] << 8 | data[4 * 2 + 1];
}

LiquidCrystal_PCF8574 lcd(0x27);
HM330X sensor_hm330x;
SHT35 sensor_sht35(SCLPIN);

void setup() {
  SERIAL_OUTPUT.begin(115200);

  // init LCD Display
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  delay(100);

  // init HM330X and output to serial
  if (sensor_hm330x.init()) {
    SERIAL_OUTPUT.println("HM330X failed to initialize");
  }

  if (sensor_sht35.init()) {
    SERIAL_OUTPUT.println("SHT35 failed to initialize");
  }
  
  SERIAL_OUTPUT.println("PM2.5 PM10");
}

void loop() {
  u16 value = 0;
  u8 data[6] = {0};
  float temp, humidity;
  
  if (sensor_hm330x.read_sensor_value(buf, 29)) {
    SERIAL_OUTPUT.println("HM330X read fail");
    lcd.home();
    lcd.clear();
    lcd.print("HM330X read fail");
  }

  if (NO_ERROR != sensor_sht35.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, &temp, &humidity)) {
        SERIAL_OUTPUT.println("SHT35 read fail");
        lcd.home();
        lcd.clear();
        lcd.print("SHT35 read fail");
  }

  int PM2_5 = getPM2_5(buf);
  int PM10 = getPM10(buf);

  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PM 2.5: ");
  lcd.print(PM2_5);
  lcd.setCursor(0, 1);
  lcd.print("PM 10: ");
  lcd.print(PM10);

  SERIAL_OUTPUT.print(PM2_5);
  SERIAL_OUTPUT.print(" ");
  SERIAL_OUTPUT.println(PM10);

  delay(2000);

  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("\xDF""C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  lcd.print(humidity);
  lcd.print("%");

  delay(2000);
}
