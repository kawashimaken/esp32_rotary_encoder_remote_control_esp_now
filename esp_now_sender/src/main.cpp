#include <esp_now.h>
#include <WiFi.h>

// ==========================================================================
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
// you the program find_mac_address in this repo to find your ESP32's MAC address 
uint8_t broadcastAddress1[] = {0x??, 0x??, 0x??, 0x??, 0x??, 0x??};
// if you have other receivers you can define in the following
// uint8_t broadcastAddress2[] = {0xFF, , , , , };
// uint8_t broadcastAddress3[] = {0xFF, , , , , };

// ==========================================================================
// your data structure (the data block you send to receiver)
// your reciever should use the same data structure
typedef struct data_structure {
  int newPosition;
  bool isButtonPressed;
} data_structure;

data_structure sending_data;

//=============================================================================
// some global variables
int newPosition = 15;
int oldPosition = 0;
bool isButtonPressed = false;


//=============================================================================
// Sender Rotary Encoder related
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

/*
  connecting Rotary encoder

  Rotary encoder side    ESP32 side
  -------------------    ---------------------------------------------------------------------
  CLK (A pin)            any microcontroler intput pin with interrupt -> 32
  DT (B pin)             any microcontroler intput pin with interrupt -> 39
  SW (button pin)        any microcontroler intput pin with interrupt -> 17
  GND                    ESP32 GND
  VCC                    ESP32 3V
*/
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 39
#define ROTARY_ENCODER_BUTTON_PIN 17
#define ROTARY_ENCODER_VCC_PIN -1
//=============================================================================
#define ROTARY_ENCODER_STEPS 5

// 
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

//=============================================================================
// FastLED related
#include <FastLED.h>
// LED VCC  --> Arduino 5V you LED need power ;-)

const int DATA_PIN = 19;
const int NUM_LEDS = 16;
CRGB leds[NUM_LEDS];


void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}
void  rotary_onButtonClick()
{
  static unsigned long lastTimePressed = 0;
  //ignore multiple press in that time milliseconds
  if (millis() - lastTimePressed < 500)
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed at ");
  Serial.println(millis());
  isButtonPressed = true;
  sending_data.newPosition = newPosition;
  sending_data.isButtonPressed = isButtonPressed;

  esp_err_t result = esp_now_send(0, (uint8_t *) &sending_data, sizeof(data_structure));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);

}


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // register peer
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // ==========================================================================
  // initialize rotary encoder
  rotaryEncoder.begin();

  attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN), readEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN), readEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_BUTTON_PIN), rotary_onButtonClick, RISING);

  // set boundaries and if values should cycle or not
  // in this example we will set possible values between 0 and 1000;
  bool circleValues = true;
  // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.setBoundaries(0, 15, circleValues); 
  // set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
  rotaryEncoder.setAcceleration(0); 

  // ==========================================================================
  // FastLED related configuration
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  set_max_power_in_volts_and_milliamps(5, 100);
  FastLED.show();
}


// ==========================================================================
// should be the same with sender
void led_animation(){
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i == newPosition) {
        leds[i] = CHSV((newPosition) * 15, 16, 16);
      } else {
        leds[i] = CHSV((newPosition) * 15, 255, 255);
      }
    }
    FastLED.show();
}

void rotary_loop()
{
  // dont do anything unless value changed
  if (!rotaryEncoder.encoderChanged())
  {
    return;
  } else {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());
    newPosition = rotaryEncoder.readEncoder();

    Serial.println(newPosition);
    sending_data.newPosition = newPosition;
    sending_data.isButtonPressed = false;
    esp_err_t result = esp_now_send(0, (uint8_t *) &sending_data, sizeof(data_structure));

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    led_animation();
  }
}
//  MAIN LOOP
void loop() {
  rotary_loop();
  if (isButtonPressed == true) {
    Serial.print(newPosition);
    isButtonPressed = false;
  }
}
//=============================================================================
// END


