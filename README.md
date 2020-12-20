# CO2 Traffic Light
Traffic light for DIY CO2 sensor

## 7-segment version
![Block diagram of CO2 sensor setup (7-segment version)](https://github.com/Alasterer/CO2_traffic_light_7_segment/blob/main/7-seg_version_V1_block_diagram.png) 

DIY CO2 sensor with the following features:
  * CO2 sensor (Telaire T6713) for measuring real air CO2 content (it is not a equivalent CO2 sensor)
  * 7-segment display to show current CO2 measurement in ppm (unit for CO2)
  * RGB LED as traffic light to show the current air quality assessment
    * 0 to 1600 ppm: green light, air quality is good
    * 1600 to 2200 ppm: yellow light, air quality could be better, please air the room
    * above 2200 ppm: red light, air quality is bad, concentration problems could occur above this threshold, air room as soon as possible
  * button with 2 functions:
    * short press (< 1 s): change brightness of RGB LED and 7-segment display. Useful to adjust brightness for use at night or with direct sunlight.
    * long press (>= 10 s): start and perform calibration of CO2 sensor (Telaire T6713). Calibration takes about 7 to 8 minutes to complete.
  * Central microcontroller (Arduino Nano) outputs new CO2 ppm value as simulated serial port with 115200 baud. Can be accessed, recorded and plotted with e.g. Python script.
