#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include "../lib/FastLED/FastLED.h"
#include "../lib/cJSON/cJSON.h"

// Define SPI
#define DATA_PIN 7
#define CLOCK_PIN 5

// Define LED values
#define NUM_LEDS 4
CRGB leds[NUM_LEDS];
#define MAX_LED_BRIGHTNES 100

// Define general parameters
const int loops_until_next_poll = 3;  // number of loops that are passing until the current iss location is requested from the server (one loop is 10 sec)
const int time_until_wifi_issue_is_illuminated = 5; // time [s] until the LEDs turn pink to signal a connection issue
const double max_sight_dist_to_iss = 1000000; // distance at which the LEDs are illuminated with a running light (theoretical max dist to see ISS: 2294000 [m])

// Wifi
const char *ssid = "ssid";  // SSID of the Wifi
const char *password = "pw";  // Password of the Wifi

// Own Position GPS position (positive numbers mean N/E negative S/W)
const double own_longitude = 6.0;
const double own_latitude = 51.0;

// other member variables
bool issInSight = false;
double own_cartesian_pos_x = 0.0;
double own_cartesian_pos_y = 0.0;

void connectingWifiIllumination() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].red = 237;
    leds[i].green = 26;
    leds[i].blue = 255;
    leds[i].maximizeBrightness(MAX_LED_BRIGHTNES);
    FastLED.show();
  }
}

void connectToWifi() {
  //preventing reconnecting issue (taking too long to connect)
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //hiding the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);

  // connect to router
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  // Wait for connection
  int num_reconnection_attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if(num_reconnection_attempts > 5) {
      connectingWifiIllumination();
    }
    num_reconnection_attempts++;
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void illuminateISS(bool iss_active) {
  // running light
  if(iss_active) {
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i].red = 255; leds[i].green = 255; leds[i].blue = 100;
        leds[i].maximizeBrightness(MAX_LED_BRIGHTNES);
        FastLED.show();
      for(int j = 0; j < NUM_LEDS; j++) {
        if(j == i) {
          continue;
        }
        leds[j].red = 255; leds[j].green = 60; leds[j].blue = 0;
        leds[j].maximizeBrightness(MAX_LED_BRIGHTNES/2);
        FastLED.show();
      }
      delay(300);
    }
  }
  // reset colors
  for(int j = 0; j < NUM_LEDS; j++) {
    leds[j].red = 255; leds[j].green = 60; leds[j].blue = 0;
    leds[j].maximizeBrightness(MAX_LED_BRIGHTNES/2);
    FastLED.show();
  }
}

void calculateEquirectualProjection(double latitude, double longitude, double& x, double& y) {
  // calculate the position on the earth 
  double radiusOfEarth = 6371000.785; // mean radius of earth in [m] 
  x = radiusOfEarth * longitude/static_cast<double>(180.0) * std::cos(own_latitude/static_cast<double>(180.0));
  y = radiusOfEarth * latitude/static_cast<double>(180.0);
}

bool issIsInSight() {
  // reconnect Wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from Wifi - Reconnecting now!");
    connectToWifi();
  }
  HTTPClient http;
  //GET ISS position
  String Link = "http://api.open-notify.org/iss-now.json";
  http.begin(Link);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();

  // parsing position
  cJSON *json = cJSON_Parse(payload.c_str());
  cJSON *iss_pos = cJSON_GetObjectItemCaseSensitive(json, "iss_position");
  cJSON *longitude_json = cJSON_GetObjectItemCaseSensitive(iss_pos, "longitude");
  cJSON *latitude_json = cJSON_GetObjectItemCaseSensitive(iss_pos, "latitude");
  char **endptr;
  double iss_longitude = strtod(longitude_json->valuestring, endptr);
  double iss_latitude = strtod(latitude_json->valuestring, endptr);

  // calculate current cartesian position of ISS
  double iss_cartesian_pos_x, iss_cartesian_pos_y;
  calculateEquirectualProjection(iss_latitude, iss_longitude, iss_cartesian_pos_x, iss_cartesian_pos_y);

  // check if ISS is in sight
  double vec_between_iss_and_me_x = own_cartesian_pos_x - iss_cartesian_pos_x;
  double vec_between_iss_and_me_y = own_cartesian_pos_y - iss_cartesian_pos_y;
  if(std::sqrt(std::pow(vec_between_iss_and_me_x, 2.0) + std::pow(vec_between_iss_and_me_y, 2.0)) < max_sight_dist_to_iss){
      return true;
  }
  return false;
}

void setup() {
  // init serial
  Serial.begin(9600);

  // init LEDs for FastLED API
  FastLED.addLeds<APA102,DATA_PIN,CLOCK_PIN,BGR>(leds,NUM_LEDS);

  delay(1000);
  connectingWifiIllumination();
  connectToWifi();

  // calculate own position in cartesian coordinates
  calculateEquirectualProjection(own_latitude, own_longitude, own_cartesian_pos_x, own_cartesian_pos_y);
}

void loop() {
  static int loopcounter = 0;
  loopcounter++;
  static bool issInSight = false;
  
  if(loopcounter > loops_until_next_poll) {
      issInSight = issIsInSight();
      loopcounter = 0;
  }
  illuminateISS(issInSight);
  delay(10000);
}
