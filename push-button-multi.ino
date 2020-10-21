/*
   ESP8266 Wi-Fi Button
   Article on code: https://techlife.nz/blog/battery-powered-esp-01-push-button/
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pwr_pin 15

const char* deviceName = "esp-push-button";

// Replace with your SSID and Password
const char* ssid     = "ketchup";
const char* password = "***";

const char* mqtt_server = "192.168.1.1";



WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  pinMode(pwr_pin, OUTPUT);
  digitalWrite(pwr_pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.begin(115200);

  initWifi();

  mqttConnection();

}

void loop() {
  // sleeping so wont get here
}

// Establish a Wi-Fi connection with your router
void initWifi() {

  Serial.print("\nConnecting to: ");
  Serial.print(ssid);
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi, going back to sleep");
    ESP.deepSleep(0);
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.println("msec");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT message

void mqttConnection() {  
  int retries = 0;

  client.setServer(mqtt_server, 1883);

  while (!client.connected()){
  Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("MQTT Connected");
    }
    else {
      retries ++;

      if (retries >= 5) {
        Serial.println("MQTT Failed 5 times. Going to sleep...");
        ESP.deepSleep(0);
      }
      delay(2000);
    }
  }

  client.publish("switch/btn1", "toggle");
  client.publish("switch/btn", "btn1");

  Serial.println("Closing the MQTT connection");
  client.disconnect();
  ESP.deepSleep(0);
  delay(2000);

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)

}
