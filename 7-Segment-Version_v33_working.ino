#include <Arduino.h>
#include <TM1637Display.h>  // include for 7-segment display TM1637
#include <Wire.h>           // include for CO2 Sensor Telaire T67xx (i have T6713)
#include <FastLED.h>        // inclue for RGB LED WS2812B
//#include <EEPROM.h>         // for saving variables

// ----- Define Pins ----------------------------------

// 7 segment display pins
#define CLK       (byte)4
#define DIO       (byte)5

// push button pins
#define buttonpin (byte)3

// RGB LED WS2812B pins
#define RGBLEDpin (byte)8

// ----- Define parameters ----------------------------
//#define SeuilPPM1 (int)900        //1er Seuil d'alerte au CO2 (900 PPM : air bon à moyen)
#define Level_yellow (int)1600       //2nd Seuil d'alerte au CO2 (1600 PPM : air moyen à mauvais, ventillation conseillee)
#define Level_red (int)2200       //3e Seuil d'alerte au CO2 (2200 PPM : mauvais air, maux de tetes, malaise possible, ventillation indispensable)
#define T6713_Address 0x15 //T6713 i2C Address

int SensorInterval = 5500;   // Interval for reading CO2 sensor values

byte co2_offset;


unsigned long loadTime = 2000;     //temps de "chauffe" du capteur CO2
unsigned long calibrationTime = 420000;   // 7 * 60000;   // calibration take 6 min so we set 7 min to be sure that calibration is finished
int loadSpeed = 10;       //rapiditée de l'animation de chargement
int messageInterval = 4;  //fréquence des messages d'alerte si un seuil CO2 est passé (4 par défaut)
bool button_state;

uint8_t CO2[] = { 0x39, 0x5c, 0x5b, 0x00 };   // code for showing "CO2"  on 7-segment display
uint8_t CAL[] = { 0x39, 0x77, 0x38, 0x00 };   // code for showing "CAL"  on 7-segment display
uint8_t Done_Display[] = { 0x5E, 0x5c, 0x54, 0x79 };  // code for showing "DONE" on 7-segment display

// init variables and objects   
TM1637Display displayScreen(CLK, DIO);
unsigned long getDataTimer = 20000;
int CO2PPM = 0;
int messageBox = 0;

CRGB leds[1];     // Define variables for RGB LED WS2812B
byte led_state = 0;             //0=off, 1=green, 2=yellow, 3=red, 4=blue, 5=pink
byte previous_led_state = 0;    //0=off, 1=green, 2=yellow, 3=red, 4=blue, 5=pink
//byte overall_brightness;    //1=lowest , 2=normal , 3=high , 4=very high brightness
int overall_brightness = 1;    //1=lowest , 2=normal , 3=high , 4=very high brightness
float brightness = 2.0;
byte brightness_RGB_LED = 15;



// ----- Setup ---------------------------------------//

void setup()
{
  // init and set input and output pins
  pinMode(buttonpin, INPUT_PULLUP);
  pinMode(RGBLEDpin, OUTPUT);
  digitalWrite(RGBLEDpin, LOW);     // init RGB LED data channel with LOW

  // init and start serial
  Serial.begin(115200);

  // start I2C communication
  Wire.begin();  

  // init and start EEPROM library
  //EEPROM.begin(4);        // start reading EEPROM and use only first 4 bytes or EEPROM memory (4096 byte would be possible)

  // Start RGB LED
  FastLED.addLeds<WS2812B, RGBLEDpin, RGB>(leds, 1);
  clearLED();

  // initialize and set 7-seg display and RGB LED brightnesses
  //init_brightness();
  set_brightness();             // increases overall_brightness to 2 and sets the "normal" setting

  // set 7-seg display
  uint8_t clear_lcd_data[] = {0x40, 0x40, 0x40, 0x40};
  displayScreen.setSegments(clear_lcd_data);                  // writes only dashes to 7-segment display

  //readSensor();
  //loadingAnimation();                                         // Start loading animation

  // Start CO2 sensor
  turnonABCLogic();

  // print welcome message
  //Serial.println(F("Sensor stared and active!"));
}

// ----- MAIN PROGRAM ---------------------------------------//

void loop() {
  button_state = digitalRead(buttonpin);
  if ( button_state == 0 ){
    delay(500);
    button_state = digitalRead(buttonpin);
    if ( button_state == 1 ){
      previous_led_state = led_state; 
      for(int i=0; i<2; i++){
        setLED_pink();
        delay(100);
        clearLED();
        delay(100);        
      }            
      set_brightness();
    } else{
      delay(9500);
//      previous_led_state = led_state;
//      for(int i=0; i<15; i++){
//        setLED_blue();
//        delay(100);
//        clearLED();
//        delay(100);        
//      }
//      led_state = previous_led_state;
      button_state = digitalRead(buttonpin);
      if ( button_state == 0 ){
        displayScreen.setSegments(CAL);
        setLED_blue();
        //delay(7000);      
        loadTime = calibrationTime;
        loadSpeed = 420;       //rapiditée de l'animation de chargement
        calibrateSensor();
        //loadingAnimation();
        //calibrationAnimation();      
        clearLED();
        displayScreen.setSegments(Done_Display);
        getDataTimer = millis();
      }
    }
  }

  if (millis() - getDataTimer >= SensorInterval)
  {
    getDataTimer = millis();
    //CO2PPM = CO2PPM + 200;
    // getStatus_short_output();
    CO2PPM = readSensor() + co2_offset;
    // getStatus_short_output();
    if (CO2PPM > 370) {
      Serial.println(CO2PPM);
      if ( CO2PPM < Level_yellow){
        setLED_green();
      } else if ( CO2PPM >= Level_yellow && CO2PPM < Level_red ){
        setLED_yellow();
      } else {
        setLED_red();
      }
      //displayScreen.showNumberDecEx(CO2PPM, false);
      displayScreen.showNumberDec(CO2PPM);
    }
  }
}


// --------------------------------------------------------//

//void calibrationAnimation() {
//  uint8_t loadA[] = { 0x39, 0x77, 0x38, 0x01 };                //loading animation 1/6
//  uint8_t loadB[] = { 0x39, 0x77, 0x38, 0x02 };                //loading animation 2/6
//  uint8_t loadC[] = { 0x39, 0x77, 0x38, 0x04 };                //loading animation 3/6
//  uint8_t loadD[] = { 0x39, 0x77, 0x38, 0x08 };                //loading animation 4/6
//  uint8_t loadE[] = { 0x39, 0x77, 0x38, 0x10 };                //loading animation 5/6
//  uint8_t loadF[] = { 0x39, 0x77, 0x38, 0x20 };                //loading animation 6/6
//
//  int delay_per_segment = 400;
//  calibrationTime = 4800;
//  int runtime = (int)calibrationTime / (6 * delay_per_segment);
//  for (int i = 0; i <= runtime; i++) {
//    displayScreen.setSegments(loadA);
//    delay(delay_per_segment);
//    displayScreen.setSegments(loadB);
//    delay(delay_per_segment);
//    displayScreen.setSegments(loadC);
//    delay(delay_per_segment);
//    displayScreen.setSegments(loadD);
//    delay(delay_per_segment);
//    displayScreen.setSegments(loadE);
//    delay(delay_per_segment);
//    displayScreen.setSegments(loadF);
//    delay(delay_per_segment);
//  }
//  displayScreen.clear();
//}

void loadingAnimation()
{
  uint8_t loadA[] = { 0x30, 0x00, 0x00, 0x00 };                //animation de chargment 1/5
  uint8_t loadB[] = { 0x06, 0x30, 0x00, 0x00 };                //animation de chargment 2/5
  uint8_t loadC[] = { 0x00, 0x06, 0x30, 0x00 };                //animation de chargment 3/5
  uint8_t loadD[] = { 0x00, 0x00, 0x06, 0x30 };                //animation de chargment 4/5
  uint8_t loadE[] = { 0x00, 0x00, 0x00, 0x06 };                //animation de chargment 5/5

  //Affiche l'aniamtion en boucle
  for (int i = 0; i <= loadSpeed; i++) {
    displayScreen.setSegments(loadA);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadB);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadC);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadD);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadE);
    delay((loadTime / loadSpeed) / 5);
  }
  //uint8_t clear_lcd_data[] = {0x00, 0x00, 0x00, 0x00};
  uint8_t clear_lcd_data[] = {0x40, 0x40, 0x40, 0x40};
  displayScreen.setSegments(clear_lcd_data);
}

void setLED_red(){
  //leds[0] = CRGB( 255, 0, 0);  // Set RGB LED to green
  leds[0] = CRGB( 0, 255, 0);  // Set RGB LED to red
  //leds[0] = CRGB( 0, 0, 255);  // Set RGB LED to blue
  FastLED.setBrightness(brightness_RGB_LED);  // Set brightness to 5 (0...255 possible)
  FastLED.show();
  led_state = 3;
}

void setLED_yellow(){
  //leds[0] = CRGB( 255, 0, 0);  // Set RGB LED to green
  //leds[0] = CRGB( 0, 255, 0);  // Set RGB LED to red
  //leds[0] = CRGB( 0, 0, 255);  // Set RGB LED to blue
  leds[0] = CRGB( 190, 255, 0);  // Set RGB LED to yellow
  FastLED.setBrightness(brightness_RGB_LED);  // Set brightness to 5 (0...255 possible)
  FastLED.show();
  led_state = 2;
}

void setLED_green(){
  leds[0] = CRGB( 255, 0, 0);  // Set RGB LED to green
  //leds[0] = CRGB( 0, 255, 0);  // Set RGB LED to red
  //leds[0] = CRGB( 0, 0, 255);  // Set RGB LED to blue
  FastLED.setBrightness(brightness_RGB_LED);  // Set brightness to 5 (0...255 possible)
  FastLED.show();
  led_state = 1;  
}

void setLED_blue(){
  //leds[0] = CRGB( 255, 0, 0);  // Set RGB LED to green
  //leds[0] = CRGB( 0, 255, 0);  // Set RGB LED to red
  leds[0] = CRGB( 0, 0, 255);  // Set RGB LED to blue
  FastLED.setBrightness(brightness_RGB_LED);  // Set brightness to 5 (0...255 possible)
  FastLED.show(); 
  led_state = 4; 
}

void setLED_pink(){
  //leds[0] = CRGB( 255, 0, 0);  // Set RGB LED to green
  //leds[0] = CRGB( 0, 255, 0);  // Set RGB LED to red
  //leds[0] = CRGB( 0, 0, 255);  // Set RGB LED to blue
  leds[0] = CRGB( 255, 84, 167);  // Set RGB LED to blue
  FastLED.setBrightness(brightness_RGB_LED);  // Set brightness to 5 (0...255 possible)
  FastLED.show(); 
  led_state = 5; 
}

void clearLED(){
  FastLED.clear();  // clear all pixel data
  FastLED.show();
  led_state = 0;
}

// Read current sensor value of Telaire T6713
int readSensor(){
  // start I2C
  Wire.beginTransmission(T6713_Address);
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x8B); Wire.write(0x00); Wire.write(0x01);
  Wire.endTransmission();
  
  // read report of current gas measurement in ppm
  delay(5);  
  Wire.requestFrom(T6713_Address, 4); //Request 4 bytes from the sensor
  int data [4];  
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
//  Serial.print("FUNCTION CODE: ");
//  Serial.println(data[0], HEX);
//  Serial.print("BYTE COUNT: ");
//  Serial.println(data[1], HEX);
//  Serial.print("MOST SIGNIFICANT BIT (HEX): ");
//  Serial.println(data[2],HEX);
//  Serial.print("LEAST SIGNIFICANT BIT (HEX): ");
//  Serial.println(data[3],HEX);
  int ppmValue = ((data[2] & 0x3F ) << 8) | data[3];
  return ppmValue;
}

// Calibrate sensor Telaire T6713
void calibrateSensor(){
  Serial.println(F("Calibrating sensor, please wait..."));
  
  // start I2C and request to start Single Point Calibration (I2C)
  Wire.beginTransmission(T6713_Address);  // T6713_Address = 0x15 by default
  Wire.write(0x05); Wire.write(0x03); Wire.write(0xEC); Wire.write(0xFF); Wire.write(0x00);
  Wire.endTransmission();

  // read Modbus Response (I2C) during calibration to see if calibration has finished
  int counter = 48;   // wait for 8 min to be sure that calibration has finished
  while(counter > 0){
    delay(10000);  
    getStatus_short_output();
    counter = counter - 1;
  }

  Serial.println(F("Calibration: DONE"));
}


// Turn on ABC Logic for sensor Telaire T6713 and read back result
void turnonABCLogic_with_debug_output(){
  // start I2C and request to enable ABC Logic
  turnonABCLogic();
  
  // read Modbus Response (I2C) to command "enable ABC Logic"
  Wire.requestFrom(T6713_Address, 5); //Request 5 bytes from the sensor
  int data [5]; 
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
  data[4] = Wire.read();
  Serial.print(F("Turning on ABC logic: FUNCTION CODE: "));
  Serial.print(data[0], HEX);
  Serial.print(F(", Register Address (MSB): "));
  Serial.print(data[1], HEX);
  Serial.print(F(", Register Address (LSB): "));
  Serial.print(data[2],HEX);
  Serial.print(F(", Register Value (MSB) Enable ABC Logic: "));
  Serial.print(data[3],HEX);
  Serial.print(F(", Register Value (LSB) LSB will always be zero: "));
  Serial.println(data[4],HEX);
}


// Turn on ABC Logic for sensor Telaire T6713
void turnonABCLogic(){
  // start I2C and request to enable ABC Logic
  Wire.beginTransmission(T6713_Address);  // T6713_Address = 0x15 by default
  Wire.write(0x05); Wire.write(0x03); Wire.write(0xEE); Wire.write(0xFF); Wire.write(0x00);
  Wire.endTransmission();
  delay(100);
}


//Get firmware version of sensor Telaire T6713
void getFirmwareVersion(){
  // start I2C and request firmware version
  Wire.beginTransmission(T6713_Address);  // T6713_Address = 0x15 by default
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x89); Wire.write(0x00); Wire.write(0x01);
  Wire.endTransmission();
  delay(100);
  
  // read Modbus Response (I2C) to command "enable ABC Logic"
  int data [4];
  Wire.requestFrom(T6713_Address, 4); //Request 4 bytes from the sensor
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
  Serial.print(F("Firmware version is (in HEX): 0x"));
  Serial.println(data[2],HEX);
  Serial.println(data[3],HEX);
//  Serial.print(F("FUNCTION CODE (should be HEX 0x04): "));
//  Serial.println(data[0], HEX);
//  Serial.print(F("Byte Count (should be HEX 0x02): "));
//  Serial.println(data[1], HEX);
//  Serial.print(F("Status (MSB): "));
//  Serial.println(data[2],HEX);
//  Serial.print(F("Status (LSB): "));
//  Serial.println(data[3],HEX);
}

// Get current status of sensor Telaire T6713
void getStatus(){
  // start I2C and request firmware version
  Wire.beginTransmission(T6713_Address);  // T6713_Address = 0x15 by default
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x8A); Wire.write(0x00); Wire.write(0x01);
  Wire.endTransmission();
  delay(100);
  
  // read Modbus Response (I2C) to command "enable ABC Logic"
  int data [4];
  Wire.requestFrom(T6713_Address, 4); //Request 4 bytes from the sensor
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
  Serial.println(F("RESPONSE: "));
  Serial.print(F("FUNCTION CODE (should be HEX 0x04): "));
  Serial.println(data[0], HEX);
  Serial.print(F("Byte Count (should be HEX 0x02): "));
  Serial.println(data[1], HEX);
  Serial.print(F("Status (MSB): "));
  Serial.println(data[2],HEX);
  Serial.print(F("Status (LSB): "));
  Serial.println(data[3],HEX);
}

// Get current status of sensor Telaire T6713
void getStatus_short_output(){
  // start I2C and request firmware version
  Wire.beginTransmission(T6713_Address);  // T6713_Address = 0x15 by default
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x8A); Wire.write(0x00); Wire.write(0x01);
  Wire.endTransmission();
  delay(100);
  
  // read Modbus Response (I2C) to command "enable ABC Logic"
  int data [4];
  Wire.requestFrom(T6713_Address, 4); //Request 4 bytes from the sensor
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();
  Serial.print(data[0], HEX);
  Serial.print(F(" / "));
  Serial.print(data[1], HEX);
  Serial.print(F(": "));
  Serial.print(data[2],HEX);
  Serial.println(data[3],HEX);
}

//void init_brightness(){
//  overall_brightness = EEPROM.read(1);
//}

void set_brightness(){
  overall_brightness = overall_brightness + 1;
  if (overall_brightness == 1){
    brightness = 0.1;           //out of 7.0
    brightness_RGB_LED = 1;     //out of 255
  } else if (overall_brightness == 2){
    brightness = 2.0;           //out of 7.0
    brightness_RGB_LED = 15;    //out of 255    
  } else if (overall_brightness == 3){
    brightness = 4.0;           //out of 7.0
    brightness_RGB_LED = 30;    //out of 255     
  } else if (overall_brightness == 4){
    brightness = 7.0;           //out of 7.0
    brightness_RGB_LED = 255;   //out of 255 
  } else {      // overflow case --> reduce overall_brightness to 1
    overall_brightness = 1;
    brightness = 0.1;           //out of 7.0
    brightness_RGB_LED = 1;    //out of 255
  }
  displayScreen.setBrightness(brightness);      // set brightness of 7-segment display
  displayScreen.showNumberDec(CO2PPM);          // show current measurement value
  FastLED.setBrightness(brightness_RGB_LED);    // set brightness of RGB LED
  set_led_to_previous_state();
}

void set_led_to_previous_state(){
  if (previous_led_state == 0){
    clearLED();
  } else if (previous_led_state == 1){
    setLED_green();
  } else if (previous_led_state == 2){
    setLED_yellow();
  } else if (previous_led_state == 3){
    setLED_red();
  } else if (previous_led_state == 4){
    setLED_blue();
  } else if (previous_led_state == 5){
    setLED_pink();
  } else {
    clearLED();
  }
}
