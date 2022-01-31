//ota library header  start 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
const char* ssid = "Super Home 8";
const char* password = "neways@12345";
AsyncWebServer server(80);
//ota end 

//firebase header start 
#include "FirebaseESP8266.h"
#define FIREBASE_HOST "https://smarttv-b9325-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Bnw62qhpYqlow4GyuamEjM6eLEjQdcOf4NhL0bMA"
int data;
FirebaseData fbdo;
int last_minute, hh, mm;
int last_hh, last_mm, timemin, timef, timer;
String savedata;
int saveval;
//firebase  header  end 

// time read and ir header start  
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int currentHour, currentMinute;
bool check_status = true;
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
const uint16_t kRecvPin = 14;
const uint16_t kIrLedPin = 4;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;  // 1024 == ~511 bits
const uint8_t kTimeout = 50;  // Milli-Seconds
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.
IRsend irsend(kIrLedPin);
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
decode_results results;
// time read and ir header end 
// This section of code runs only once at start-up.
int k;




//tv on function start 
void tv_on()
{

  results.value = 0x40BE629D;
  for (int i = 0; i < 5; i++)

  {
    irsend.send(NEC, results.value, 32);
    delay(10);
  }

  Serial.println("tv is onnn delay end ");
  delay(50000);

}

//tv on function end





//tv off function start 

void tv_off()
{
  results.value = 0x40BE629D;
  for (int i = 0; i < 5; i++)
  {
    irsend.send(NEC, results.value, 32);
    delay(10);
  }
  Serial.println("tv is onnn delay end ");
}
//tv off function end






// tp app run function  start 

void run_app()
{
  Serial.println("run app");
  Serial.println("changing");
  results.value = 0x40BEB24D;
  for (int i = 0; i < 6; i++)

  {
    irsend.send(NEC, results.value, 32);
    delay(200);
  }

  delay(1000);
  results.value = 0x40BE52AD;
  Serial.println("ok button is pressed ");
  for (int i = 0; i < 6; i++)

  {
    irsend.send(NEC, results.value, 32);
    delay(200);
  }
  Serial.println("run complete");
  delay(1000);

}

// tp app run function  end

//ota start function 
void ota()
{

Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("wifi connectring ");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(300);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "smart tv branch 8.");

  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
  }

  //ota end function 




//database function is start 

void database()
{
    if(last_mm<mm)
    { 
      Serial.println("insert data ");
    savedata="/BAR_100920_294562129482664344_1599721915/";
    savedata+=String(k);
    saveval=(hh*60)+mm;
    Firebase.setInt(fbdo,savedata,saveval);
    last_mm=mm;
    last_mm+=5;
    k++;
    }
  if (Firebase.getInt(fbdo, "/BAR_100920_294562129482664344_1599721915/hh/"))
  {
    //Success
    Serial.print("Get int data success, int = ");
    hh = fbdo.intData();
    Serial.println(hh);

  } else {
    //Failed?, get the error reason from fbdo
    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.getInt(fbdo, "/BAR_100920_294562129482664344_1599721915/mm/"))
  {
    //Success
    Serial.print("Get int data success, int = ");
    mm = fbdo.intData();
    Serial.println(mm);

  } 
  else {
  
    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }

  
 }





//database function is end 




//check time function is start 

void check_time()
{
  digitalWrite(D0,1);
  delay(500);
  
  digitalWrite(D0,0);
  delay(100);
  timeClient.update();
  /* String formattedTime = timeClient.getFormattedTime();
    Serial.print("Formatted Time: ");
    Serial.println(formattedTime);
  */
  currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);
  currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute);
  delay(2000);

}

//check function is end 





void setup() {

  
  ota();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);
  irrecv.enableIRIn();  // Start up the IR receiver.
  irsend.begin();       // Start up the IR sender.

  Serial.begin(kBaudRate, SERIAL_8N1);
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();

  Serial.print("SmartIRRepeater is now running and waiting for IR input "
               "on Pin ");
  Serial.println(kRecvPin);
  Serial.print("and will retransmit it on Pin ");
  Serial.println(kIrLedPin);
  timeClient.begin();
  timeClient.setTimeOffset(21600);
}



void loop() {

  check_time();
  database();
  timef = hh * 60 + mm;
  timer = currentHour * 60 + currentMinute;
  Serial.println(timef);
  Serial.println(timer);
  if (abs(timer - timef) > 35)
  {
    tv_on();
    run_app();
    Serial.println("need to turn on");
  }

  else
  {

    Serial.println("tv is ON");
    
  }


  if (24<currentHour >= 6 && data == 0  )
  {
    Serial.println("on funtion is called ");
    tv_on();
    run_app();
    //get firebase time
    //database();
    //check_time();
  }

  if (currentMinute == 0 &&  currentHour == 0 )
  {

    Serial.println("off funtion is called ");
    tv_off();
    check_status = true;
    Serial.println("waiting :::::::::::");
    delay(70000);
    Serial.println("waiting ::::::::::: end ");
  }

  
  delay(10000);

  yield();
}
