# CO2 Traffic Light
DIY CO2 sensor with traffic light and 7-segment display for air quality indication

<p float="left">
 <img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_FRONT.jpg" width="500"/>
 <img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_BACK.jpg" width="250"/>
 <img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/CO2_traffic_light_7-segment_version_INTERNALS.jpg" width="250"/>
</p>

## 7-segment version

DIY CO2 sensor with the following features:
  * CO2 sensor (Telaire T6713) for measuring real air CO2 content (it is not a equivalent CO2 sensor)
  * 7-segment display (TM1637, 4 digits) to show current CO2 measurement in ppm (unit for CO2)
  * RGB LED (WS2812B) as traffic light to show the current air quality assessment
    * 0 to 1600 ppm: green light, air quality is good
    * 1600 to 2200 ppm: yellow light, air quality could be better, please air the room
    * above 2200 ppm (max for sensor is 5000 ppm): red light, air quality is bad, concentration problems could occur above this threshold, air room as soon as possible
  * button with 2 functions:
    * short press (< 1 s): change brightness of RGB LED and 7-segment display. Useful to adjust brightness for use at night or with direct sunlight.
    * long press (>= 10 s): start and perform calibration of CO2 sensor (Telaire T6713). Calibration takes about 7 to 8 minutes to complete.
  * Central microcontroller (Arduino Nano) outputs new CO2 ppm value as simulated serial port with 115200 baud. Can be accessed, recorded and plotted with e.g. Python script.

<img src="https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/7-seg_version_V1_block_diagram.png" width="400"/>

---

## LCD display version

It is planned to complete a version with LCD display (Nokia 5110) that has several more features:
  * Nokia 5110 LCD display (has background LED)
  * Push buttons, 3x, SMD: For navigating on screen menu on LCD screen
  * Night light LED (warm white) for soft room illumination at night
  * Photoresistor for detection ambient light levels (can be used to dim RGB LED, Night light LED and/or display backlight)
  * Connector for BMP180 sensor module: Can measure temperature and ambient barometric pressure. Values will be display on LCD as well
  * Connector for NRF24L01 communication module:
    * Use case 1: Communicate with separate Arduino that activates fan to vent room when CO2 levels are too high
    * Use case 2: Communicate with Raspberry Pi for recording measurement values
