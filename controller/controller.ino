#include <LiquidCrystal_PCF8574.h>
#include <Seeed_HM330X.h>

LiquidCrystal_PCF8574 lcd(0x27);
HM330X sensor;

#ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL_OUTPUT SerialUSB
#else
    #define SERIAL_OUTPUT Serial
#endif

uint8_t buf[30];

int getPM2_5(uint8_t* data) {
    if (data == NULL) {
        return ERROR_PARAM;
    }
    return data[3 * 2] << 8 | data[3 * 2 + 1];
}

int getPM10(uint8_t* data) {
    if (data == NULL) {
        return ERROR_PARAM;
    }
    return data[4 * 2] << 8 | data[4 * 2 + 1];
}

void setup() {
    SERIAL_OUTPUT.begin(115200);
    
    // init LCD Display
    lcd.begin(16, 2);
    lcd.setBacklight(255);
  
    delay(100);

    // init HM330X and output to serial
    if (sensor.init()) {
        SERIAL_OUTPUT.println("HM330X init failed");
        while (1);
    }
    while(!SERIAL_OUTPUT);
    SERIAL_OUTPUT.println("PM2.5 PM10");
}

void loop() {
    if (sensor.read_sensor_value(buf, 29)) {
        SERIAL_OUTPUT.println("HM330X read result failed!");
    }
    
    int PM2_5 = getPM2_5(buf);
    int PM10 = getPM10(buf);

    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PM2.5: ");
    lcd.print(PM2_5);
    lcd.setCursor(0, 1);
    lcd.print("PM10: ");
    lcd.print(PM10);
    
    SERIAL_OUTPUT.print(PM2_5);
    SERIAL_OUTPUT.print(" ");
    SERIAL_OUTPUT.println(PM10);

    delay(500);
}
