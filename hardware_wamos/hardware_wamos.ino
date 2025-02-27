
#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

// DEFINE VARIABLES
#define ARDUINOJSON_USE_DOUBLE      1 

#include <SoftwareSerial.h>
// IMPORT ALL REQUIRED LIBRARIES

#include <math.h>
   
//**********ENTER IP ADDRESS OF SERVER******************//

#define HOST_IP     "104.244.227.77"       // REPLACE WITH IP ADDRESS OF SERVER ( IP ADDRESS OF COMPUTER THE BACKEND IS RUNNING ON) 
#define HOST_PORT   "8080"            // REPLACE WITH SERVER PORT (BACKEND FLASK API PORT)
#define route       "api/update"      // LEAVE UNCHANGED 
#define idNumber    "620157584"       // REPLACE WITH YOUR ID NUMBER 

// WIFI CREDENTIALS
#define SSID        "MonaConnect"      // "REPLACE WITH YOUR WIFI's SSID"   
#define password    ""  // "REPLACE WITH YOUR WiFi's PASSWORD" 
//#define SSID        "Digicel_WiFi_eSEb"      // "REPLACE WITH YOUR WIFI's SSID"   
//#define password    "WYPx3kCk"  // "REPLACE WITH YOUR WiFi's PASSWORD" 

#define stay        100
 
//**********PIN DEFINITIONS******************//

 
#define espRX         10
#define espTX         11
#define espTimeout_ms 300
#define TRIG 5
#define ECHO 6

 
 
/* Declare your functions below */
void espSend(char command[] );
void espUpdate(char mssg[]);
float checkWaterheight();
float checkReserve();
float detpercentage();

float percentage, reserve, duration, distance, sensorheight = 94.5, maxheight = 77.763, radius = 30.75;


SoftwareSerial esp(espRX, espTX); 
 

void setup(){

  Serial.begin(115200); 
  // Configure GPIO pins here
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  espInit();  
}

void loop(){ 

  checkWaterheight();
  checkReserve();
  detpercentage();

  Serial.print(F("Radar: "));
  Serial.print(distance);
  Serial.print(F("inches\n Water Height: "));
  Serial.print(checkWaterheight());
  Serial.print(F("inches\n "));
  Serial.print(F("Reserve: "));
  Serial.print(checkReserve());
  Serial.print(F("\n Percentage: "));
  Serial.print(detpercentage());
  
  JsonDocument doc; // Create JSon object
    char message[800]  = {0};

    // Add key:value pairs to Json object according to below schema
    doc["id"]         = "620157584"; // Change to your student ID number
    doc["type"]  = "ultrasonic";
    doc["radar"]  = distance;
    doc["waterheight"]  = checkWaterheight();
    doc["reserve"]  = checkReserve();
    doc["percentage"]  = detpercentage();
    /*Add code here to insert all other variabes that are missing from Json object
    according to schema above
    */

    serializeJson(doc, message);  // Seralize / Covert JSon object to JSon string and store in char* array  
    espUpdate(message);
   
  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’
  delay(1000);  
}

 
void espSend(char command[] ){   
    esp.print(command); // send the read character to the esp    
    while(esp.available()){ Serial.println(esp.readString());}    
}


void espUpdate(char mssg[]){ 
    char espCommandString[50] = {0};
    char post[290]            = {0};

    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT); 
    espSend(espCommandString);    //starts the connection to the server
    delay(stay);

    // GET REQUEST 
    // snprintf(post,sizeof(post),"GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);

    // POST REQUEST
    snprintf(post,sizeof(post),"POST /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);
  
    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSEND=%d\r\n", strlen(post));
    espSend(espCommandString);    //sends post length
    delay(stay);
    Serial.println(post);
    espSend(post);                //sends POST request with the parameters 
    delay(stay);
    espSend("AT+CIPCLOSE\r\n");   //closes server connection
   }

void espInit(){
    char connection[100] = {0};
    esp.begin(115200); 
    Serial.println("Initiallizing");
    esp.println("AT"); 
    delay(1000);
    esp.println("AT+CWMODE=1");
    delay(1000);
    while(esp.available()){ Serial.println(esp.readString());} 

    snprintf(connection, sizeof(connection),"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,password);
    esp.print(connection);

    delay(3000);  //gives ESP some time to get IP

    if(esp.available()){   Serial.print(esp.readString());}
    
    Serial.println("\nFinish Initializing");    
   
}

//***** Design and implement all util functions below ******
float checkWaterheight()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

    // Read echo duration
  duration = pulseIn(ECHO, HIGH);
    
    // Calculate distance (cm)
  distance = (duration * 0.034) / 2;
  return sensorheight - distance;
}
float checkReserve()
{
  float waterheight = checkWaterheight();
  return 3.142 * radius * radius * waterheight * 0.004329;
}

float detpercentage()
{
  float waterheight = checkWaterheight();
  return (waterheight / maxheight) * 100;
}

