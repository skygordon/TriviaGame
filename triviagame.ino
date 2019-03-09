#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <stdio.h>
#include <list>

char network[] = "MIT";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab
//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 400; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


const int input_pin1 = 16; //pin connected to button1
const int input_pin2 = 5; //pin connected to button2
uint8_t state;  //system state for button1 press counter
uint8_t gamestate;  //system state for stage in trivia game
uint8_t num_count; //variable for storing the number of times the button has been pressed before timeout for user answer
uint8_t num_countdone; // 1 = done, 0 = counting user ans
unsigned long timer;  //used for storing millis() readings.

#define IDLE 0  //example definition
#define DOWN 1  //example definition
#define UP 2

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.begin(network,password); //attempt to connect to ESP8266WiFi
  uint8_t count = 0; //count used for ESP8266WiFi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count<12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(3000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  
  tft.init();  //init screen
  startscreen();
  tft.setRotation(3); //adjust rotation
  pinMode(input_pin1, INPUT_PULLUP); //set input pin as an input!
  pinMode(input_pin2, INPUT_PULLUP); //set input pin as an input!
  gamestate = 0; //for game play states
  num_count = 0;
  num_countdone = 0;
  state = 0; //for button1 presses
}

void loop() {
button2(digitalRead(input_pin2));
number_fsm(digitalRead(input_pin1));
}

void number_fsm(uint8_t input){
  switch(state){
    case IDLE:
      if (input == 0){
      state = DOWN;
      num_count = 0;
      }
      break; 
    case DOWN:
      if (input == 1){
      state = UP;
      num_count++; // increases num_count
      timer = millis();
      while (millis() - timer < 200) {
        }
      timer = millis();
      } 
      break;
    case UP:
    if ((millis() - timer) > 1000) {
      num_countdone = 1;
      state = IDLE;
      // checks if done pressing
    } else if (input == 0) {
      state = DOWN;
    }
      break;
  }
}

void button2(uint8_t input){
  switch(gamestate){
    case 0: //startscreen
      if (input == 0){     // will display current question
      gamestate = 1;
      state = 0;
      num_count = 0;
      num_countdone = 0;
      Serial.println(triviaapiquestion());
      }
      break; 
    case 1: // waiting for user ans
      if (num_countdone == 1){
      Serial.println(answerapi());
      num_countdone = 0;
      gamestate = 2;
      } 
      break;
    case 2: // displaying return after ans
    if (input == 0){
      gamestate = 0;
    }
      break;
  }}

  char triviaapiquestion() {
      char body[200]; //for body;
      int body_len = strlen(body); //calculate body length (for header reporting)
      sprintf(request_buffer,"GET http://608dev.net/sandbox/sc/sfgordon/triviagame.py HTTP/1.1\r\n");
      strcat(request_buffer,"Host: 608dev.net\r\n");
      strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer,"\r\n"); //new line from header to body
      strcat(request_buffer,body); //body
      strcat(request_buffer,"\r\n"); //header
      Serial.println(request_buffer);
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
      //resets screen to display question
      tft.fillScreen(TFT_BLACK); //fill background
      tft.setTextSize(1); //default font size
      tft.setCursor(0,0,1); // set the cursor
      tft.println(response_buffer); //print the trivia question
      }

  char answerapi() {
      char body[200]; //for body;
      sprintf(body,"numcount=%d",num_count);//generate body,
      int body_len = strlen(body); //calculate body length (for header reporting)
      sprintf(request_buffer,"POST http://608dev.net/sandbox/sc/sfgordon/triviagame.py HTTP/1.1\r\n");
      strcat(request_buffer,"Host: 608dev.net\r\n");
      strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer,"\r\n"); //new line from header to body
      strcat(request_buffer,body); //body
      strcat(request_buffer,"\r\n"); //header
      Serial.println(request_buffer);
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
      //resets screen accordingly
      tft.setTextSize(2); //default font size
      tft.fillScreen(TFT_BLACK); //fill background
      tft.setCursor(0,0,1); // set the cursor
      tft.println(response_buffer); //prints the result with total score
      tft.setTextSize(1);
      tft.println("");
      tft.println("Press button 2 ");
      tft.println("to keep playing!");
      }

unsigned long startscreen() {
  tft.setCursor(0, 0);
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setRotation(3); //adjust rotation
  tft.setTextSize(2); //default font size
  tft.setTextColor(TFT_CYAN);  
  tft.println("Trivia Game");
  tft.setTextSize(1); //default font size
  tft.setTextColor(TFT_GREEN); 
  tft.println("");
  tft.println("Press button 1 once");
  tft.println(" to answer True");
  tft.println("");
  tft.println("");
  tft.println("Press button 1 twice");
  tft.println(" to answer False");
  tft.setTextColor(TFT_YELLOW);
  tft.println("");
  tft.println("");
  tft.setTextSize(1); //default font size
  tft.println("to start playing press");
  tft.println(" button 2 once");
  }
