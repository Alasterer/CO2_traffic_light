# CO2 Traffic Light
DIY CO2 sensor with traffic light and 7-segment display for air quality indication

<img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_FRONT.jpg">
<p float="left">
 <img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_BACK.jpg" width="400"/>
 <img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_INTERNALS.jpg" width="400"/>
</p>

## 7-segment version

DIY CO2 sensor with the following features:
  * CO2 sensor ([Telaire T6713](https://www.14core.com/wiring-the-telaire-t6713-t67xx-a-carbon-dioxide-co2-sensor/amp), [PDF datasheet](https://www.mouser.com/pdfDocs/AAS-916-142A-Telaire-T67xx-CO2-Sensor-022719-web.pdf)) for measuring real air CO2 content (it is not a equivalent CO2 sensor)
  * 7-segment display ([TM1637](https://www.makerguides.com/tm1637-arduino-tutorial), 4 digits, [PDF datasheet controller](https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf)) to show current CO2 measurement in ppm (unit for CO2)
  * RGB LED ([WS2812B](https://www.mschoeffler.de/2017/08/24/how-to-control-an-led-pixel-strip-ws2812b-with-an-arduino-and-the-fastled-library), [PDF datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)) as traffic light to show the current air quality assessment
    * 0 to 1600 ppm: green light, air quality is good
    * 1600 to 2200 ppm: yellow light, air quality could be better, please air the room
    * above 2200 ppm (max for sensor is 5000 ppm): red light, air quality is bad, concentration problems could occur above this threshold, air room as soon as possible
  * button with 2 functions:
    * short press (< 1 s): change brightness of RGB LED and 7-segment display. Useful to adjust brightness for use at night or with direct sunlight.
    * long press (>= 10 s): start and perform calibration of CO2 sensor (Telaire T6713). Calibration takes about 7 to 8 minutes to complete.
  * Central microcontroller module (Arduino Nano, [Pinout](https://i.pinimg.com/originals/c4/87/21/c487213e9081fb0050878a02304e5693.png), [PDF datasheet microcontroller ATMega328P](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)) outputs new CO2 ppm value as simulated serial port with 115200 baud. Can be accessed, recorded and plotted with e.g. Python script.

<img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/7-seg_version_V1_block_diagram.png" width="400"/>

---

## LCD display version

It is planned to complete a version with LCD display (Nokia 5110) that has several more features:
  * [Nokia 5110 LCD display](https://learn.sparkfun.com/tutorials/graphic-lcd-hookup-guide) (has background LED), [PDF datasheet controller](https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf)
  * Push buttons, 3x, SMD: For navigating on screen menu on LCD screen
  * Night light LED (warm white, 5V, PWM dimming) for soft room illumination at night
  * Photoresistor ([GL5516](http://static.cactus.io/img/sensors/light/ldr/ldr-circuit.jpg) with 5 to 10 kOhm at 10 Lux) for detection ambient light levels (can be used to dim RGB LED, Night light LED and/or display backlight)
  * Connector for [BMP180 sensor module](http://www.esp8266learning.com/wemos-mini-bmp180-shield.php), [PDF datasheet](https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf): Can measure temperature and ambient barometric pressure. Values will be display on LCD as well
  * Connector for [NRF24L01 communication module](https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/), [PDF datasheet](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf):
    * Use case 1: Communicate with separate Arduino that activates fan to vent room when CO2 levels are too high
    * Use case 2: Communicate with Raspberry Pi for recording measurement values
