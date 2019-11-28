#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;


/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

int posx=0,posy=0,posz=0;
int touch=0;
int counter=0;
int aState;
int aLastState;  
int encoder=0;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  aLastState = digitalRead(13);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("SprkZ", "0000000000");
  Serial.println("");
}

void loop(void) 
{
  if ((WiFiMulti.run() == WL_CONNECTED)) {
  /* Get a new sensor event */ 
  HTTPClient http;
  encoder=0;
  Serial.println("[HTTP] begin...\n");
  aState = digitalRead(13); //encoder
  sensors_event_t event; 
  accel.getEvent(&event);
  if(event.acceleration.z < 5.75)
  {
    posz=1;
  }
  else if(event.acceleration.z >= 5.75)
  {
    posz=0;
  }
  if(event.acceleration.x > 2)
  {
    posx=-1;
  }
  else if(event.acceleration.x <= 2 && event.acceleration.x > -3)
  {
    posx=0;
  }
  else if(event.acceleration.x <= -3)
  {
    posx=1;
  }
  if(event.acceleration.y > 3)
  {
    posy=-1;
  }
  else if(event.acceleration.y <= 3 && event.acceleration.y > -3)
  {
    posy=0;
  }
  else if(event.acceleration.y <= -3)
  {
    posy=1;
  }
  touch=digitalRead(14);
  /* Display the results (acceleration is measured in m/s^2) */
  if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(12) != aState) { 
       counter ++;
       encoder=1;
     } else {
       counter --;
       encoder= -1;
     }
   }
   aLastState = aState; // Updates the previous state of the outputA with the current state
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  
  Serial.print("Touch : "); Serial.println(touch);
  Serial.print("Position : ");
  Serial.println(counter);
  Serial.print("encoder : ");
  Serial.println(encoder);
  Serial.print("x : "); Serial.print(posx);Serial.print(" y : "); Serial.print(posy); Serial.print(" jump : "); Serial.println(posz);
  http.begin("http://192.168.43.36:5000/?posx="+String(posx)+"&posy="+String(posy)+"&posz="+String(posz)+"&touch="+String(touch)+"&encoder="+String(encoder)); //HTTP

    Serial.println("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
  //delay(500);
  http.end();
  }
}
