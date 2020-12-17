#include <ESP8266WiFi.h>
const char *ssid = "Insert your Wifi name here"; // WiFi name
const char *password = "Insert your WiFi password here"; // WiFi password

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <math.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 6
#define CLK_PIN   D5
#define DATA_PIN  D7
#define CS_PIN    D8
MD_Parola DotMatrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
int brightness = 0;

uint8_t scrollSpeed = 70; // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textEffect_t scrollEffectOut =PA_RANDOM;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 70; // in milliseconds

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;

String  hour, minute, second;
String  year, month, date;

const char *timeLive;

//Week Days
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Month names
String months[12] = {"January", "February", "March", "April", "May", "June", "Juli", "August", "September", "October", "November", "December"};


// Buttons
#define LEFT_BTS D1
#define RIGHT_BTS D2

int pressCounterLeft;
char showDateChar[40] ="";


//Weather prequieriters
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
String openWeatherMapApiKey = "Insert you Api Key here";
String city = "Insert your Region here";
String countryCode = "Insert your country here";
String unit = "metric";
String lang = "en";
String jsonBuffer;
char showWeatherChar;

uint8_t degC[] = { 6, 3, 3, 56, 68, 68, 68 }; // Deg C

enum {TIME, DATE};
boolean displayMode = TIME;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  DotMatrix.begin();
  DotMatrix.setIntensity(0);
  DotMatrix.setCharSpacing(2);
  DotMatrix.addChar('$', degC);
  timeClient.begin();
  timeClient.setTimeOffset(3600); // Set offset time in seconds, GMT+8 = 28800

  pinMode(LEFT_BTS, INPUT);
  pinMode(RIGHT_BTS, INPUT);
  
  displayMode = DATE;
}

void loop() {
if (WiFi.status() == WL_CONNECTED) {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
    
  }
  
    if (displayMode == DATE && DotMatrix.displayAnimate()) {
      DotMatrix.displayReset();
      displayMode = TIME;
    }
  
  currentMillis = millis();

if (currentMillis - previousMillis > 600) {
        setBrightness();
        }
  
  if (currentMillis - previousMillis > interval && displayMode == TIME) {
    previousMillis = millis();
    
    
    if (digitalRead(LEFT_BTS) == HIGH) {
      
      pressCounterLeft++ ;
     
       if (pressCounterLeft == 1){
        displayMode = DATE;
        DotMatrix.displayClear();
        getExtraInfo().toCharArray(showDateChar,40);
          Serial.println("String: " + showDate());
          Serial.println(showDateChar);
          DotMatrix.displayText(showDateChar, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);;
         
          pressCounterLeft = 0;
          return;
          
       }

    } else if (pressCounterLeft == 0) {
      DotMatrix.print(showTime());
    }
  }

 } else if (WiFi.status() != WL_CONNECTED){
  DotMatrix.print("Error 599");
  }
}

String showTime() {
  String timeStamp;

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  if (currentHour == 0) {
    hour = String(currentHour + 24);
  } else {
    hour = String(currentHour);
  }

  if (currentMinute < 10) {
    minute = "0" + String(currentMinute);
  } else {
    minute = String(currentMinute);
  }

  if (currentSecond < 10) {
    second = "0" + String(currentSecond);
  } else {
    second = String(currentSecond);
  }

  timeStamp = hour + ":" + minute + ":" + second;
  return timeStamp;
}

String showDate() {
  String dateStamp;

  unsigned long epochTime = timeClient.getEpochTime();

  // Extract date
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  //String currentMonthName = months[currentMonth - 1];
  int currentYear = ptm->tm_year + 1900;
  String yearNow = String(currentYear);
  dateStamp = String(monthDay) + "." + String(currentMonth) + "." + yearNow.substring(2);
  return dateStamp;
}

void setBrightness() {
  if (digitalRead(RIGHT_BTS) == HIGH && brightness <= 15) {
    brightness++;
    Serial.println(brightness);
    DotMatrix.setIntensity(brightness);
    delay(100);
  } else if (brightness > 15) {
    brightness = 0;
  }
  
}

String getWeather(){
   if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=" + unit + "&lang=" + lang;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        
      }
      Serial.println(myObject["main"]["temp"]);
      Serial.println(myObject["weather"][0]["main"]);
      String temperature = String(JSON.stringify(myObject["main"]["temp"]));
      double tempDouble = temperature.toDouble();
      String tempString = String(tempDouble,0);
 
      String descrip = String(JSON.stringify(myObject["weather"][0]["description"]));
      String detail = descrip.substring(1,(descrip.length()-1));
      return tempString + "$ "+ detail;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   // lastTime = millis();
  }

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

String getExtraInfo(){
  String extra = showDate() + " " + getWeather();
  return extra;
  }
