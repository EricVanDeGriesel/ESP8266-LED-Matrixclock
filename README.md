# ESP8266-LED-Matrixclock
LED Matrix clock with additional date and weather and display mode.
The programm is able to display the time through a connection with a NTP server. 
A permanent connection with the interenet must exist the whole time. Via touch of a button you are able to see the actual date, the temperature of your region an short description of the weather condition. With the other buttons you can control the brightness of the LED Matrices.

The code relativles simple and easy to understand. So, because it is my very first Arduino project, i did not put to much effort in clean and changeable code. When you want to change something for your project, feel free to do it. Due to the simplicity of the code it should not be a big problem.

## Hardware
* 2 Pushbuttons
* MAX7219 LED modules
* ESP8266
* Jumpercables
* Electrical resistors

## Frameworks
* NTPClient
* MD_Parola
* MD_MAX72xx
* WiFiUdp
* ESP8266WiFi
* SPI
* Arduino_JSON
* ESP8266HTTPClient
* WiFiClient


## Installation
Make sure you installed all necessary frameworks before uploading the Sketch. Then just download the .ino file an open it with the Arduino IDE. 
Now do have make some adjustments:
* Number of 8x8 matrices
* WiFi name and password
* Openweather API key
* our location and country for the weather API call
* Language of weather description
* Timezone
 
Connect your ESP8266 with your PC and upload the sketch
