#include <esp_now.h>
#include <WiFi.h>
//=============================================================================
// indicator LED pin assignment, the LED lights up when sender button is pressed
#define LED 2


//=============================================================================
// FastLED related
#include <FastLED.h>
// LED VCC  --> Arduino 5V
const int DATA_PIN = 19;
const int NUM_LEDS = 16;
CRGB leds[NUM_LEDS];
#define FPS 30

//=============================================================================
// receive data structure
// Must match the sender structure
typedef struct data_structure {
  int newPosition;
  bool isButtonPressed;
} data_structure;

// Create a struct_message called receivedData
data_structure receivedData;

//=============================================================================
// led animation function with received ESP-NOW data
void led_animation(){
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i == receivedData.newPosition) {
        leds[i] = CHSV((receivedData.newPosition) * 15, 16, 16);
      } else {
        leds[i] = CHSV((receivedData.newPosition) * 15, 255, 255);
      }
    }
    FastLED.show();
}

//=============================================================================
// callback function that will be executed when ESP-NOW data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("newPosition: ");
  Serial.println(receivedData.newPosition);
  Serial.print("isButtonPressed: ");
  Serial.println(receivedData.isButtonPressed);
  Serial.println();

  led_animation();

  if (receivedData.isButtonPressed) {
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED, HIGH); 
    // wait for a second  
    delay(1000);               
    digitalWrite(LED, LOW);
  }
  else {
    digitalWrite(LED, LOW);
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  //===========================================================================
  // FastLED configuration
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);
  set_max_power_in_volts_and_milliamps(5, 100);
  FastLED.show();
}

void loop() {
  FastLED.show();
  FastLED.delay(1000 / FPS);
}
//=============================================================================
// END