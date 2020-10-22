/*
   ESP8266 Wi-Fi Button
   Article on code: https://techlife.nz/blog/battery-powered-esp-01-push-button/
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define btn_a 5
#define btn_b 4
#define btn_c 13
#define btn_d 14
#define btn_e 12
#define swit_cold 3
#define swit_warm 16

#define pwr_pin 1
#define beep_pin 15


const char* deviceName = "esp-push-button-multi";

// Replace with your SSID and Password
const char* ssid     = "ketchup";
const char* password = "***";

const char* mqtt_server = "192.168.1.1";



WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  pinMode(pwr_pin, OUTPUT);
  pinMode(beep_pin, OUTPUT);
  pinMode(btn_a, INPUT);
  pinMode(btn_b, INPUT);
  pinMode(btn_c, INPUT);
  pinMode(btn_d, INPUT);
  pinMode(swit_cold, INPUT);
  pinMode(swit_warm, INPUT);

  digitalWrite(pwr_pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(beep_pin, LOW);

  String mqtt_msg = "abcd";

  if ( digitalRead(btn_a) == LOW ) {
    String mqtt_msg = "btn2a";
  }
  else if ( digitalRead(btn_b) == LOW ) {
    String mqtt_msg = "btn2b";
  }
  else if ( digitalRead(btn_c) == LOW ) {
    String mqtt_msg = "btn2c";
  }
  else if ( digitalRead(btn_d) == LOW ) {
    String mqtt_msg = "btn2d";
  }
  else if ( digitalRead(btn_e) == LOW ) {
    if ( digitalRead(swit_cold) == LOW ) {
      String mqtt_msg = "btn2dc";
    }
    else if ( digitalRead(swit_warm) == LOW ) {
      String mqtt_msg = "btn2dw";
    }
    else {
      String mqtt_msg = "btn2dn";
    }
  }
  else
  {
      String mqtt_msg = "btn2_error_no_button_pressed";
  }

  digitalWrite(beep_pin, HIGH);
  delay(60);
  digitalWrite(beep_pin, LOW);


  Serial.begin(115200);

  initWifi();

  mqttConnection(mqtt_msg);

  digitalWrite(beep_pin, HIGH);
  delay(20);
  digitalWrite(beep_pin, LOW);
  delay(20);
  digitalWrite(beep_pin, HIGH);
  delay(20);
  digitalWrite(beep_pin, LOW);

}

void loop() {
  digitalWrite(pwr_pin, LOW);
  ESP.deepSleep(0);
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  // Serial.print("\nConnecting to: ");
  // Serial.print(ssid);
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(500);
    // Serial.print(".");
  }
  // Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi, going back to sleep");
    ESP.deepSleep(0);
  }

  // Serial.print("WiFi connected in: ");
  // Serial.print(millis());
  // Serial.println("msec");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
}

// MQTT message

void mqttConnection(String mqtt_msg) {
  int retries = 0;

  client.setServer(mqtt_server, 1883);

  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      //Serial.println("MQTT Connected");
    }
    else {
      retries ++;

      if (retries >= 5) {
        //Serial.println("MQTT Failed 5 times. Going to sleep...");
        ESP.deepSleep(0);
      }
      delay(500);
    }
  }
  int str_len = mqtt_msg.length() + 1; 
  char char_array[str_len];
  mqtt_msg.toCharArray(char_array, str_len);
  client.publish("switch / btn", mqtt_msg );

  // Serial.println("Closing the MQTT connection");
  client.disconnect();

}
