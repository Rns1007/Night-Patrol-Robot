#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// === WiFi Credentials ===+
const char* ssid = "Raj_iPhone";           // 🔁 Replace with your WiFi SSID
const char* password = "nipun2005";   // 🔁 Replace with your WiFi password

// === MQTT Broker Settings ===
const char* mqtt_server = "broker.emqx.io";    // ✅ Public EMQX broker
const int mqtt_port = 1883;
const char* mqtt_user = "";                    // No auth for public broker
const char* mqtt_pass = "";

// === Sensor Pins & Setup ===
#define DHTPIN 4            // DHT22 sensor connected to GPIO4
#define PIRPIN 5            // PIR sensor connected to GPIO5
#define DHTTYPE DHT11    // Use DHT11
DHT dht(DHTPIN, DHTTYPE);

// === MQTT Topics ===
const char* temp_topic = "home/esp32/temperature";
const char* hum_topic = "home/esp32/humidity";
const char* motion_topic = "home/esp32/motion";
int lastMotionState = LOW;


// === Global Objects ===
WiFiClient espClient;
PubSubClient client(espClient);

// === Timer Settings ===
unsigned long lastMsg = 0;
const long interval = 5000;

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    // Generate a unique client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Connect to EMQX (no username/password needed for public broker)
    if (client.connect(clientId.c_str()))  // No user/pass for public EMQX
 {
      Serial.println("connected");
      // No need to subscribe here since ESP32 only publishes
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(PIRPIN, INPUT);
  dht.begin();
  delay(2000);  // give time to stabilize
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    // Read temperature & humidity
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp)) {
      char tempStr[8];
      dtostrf(temp, 1, 2, tempStr);
      client.publish(temp_topic, tempStr);
    }

    if (!isnan(hum)) {
      char humStr[8];
      dtostrf(hum, 1, 2, humStr);
      client.publish(hum_topic, humStr);
    }

    // Read motion sensor (PIR)
int motion = digitalRead(PIRPIN);

if (motion != lastMotionState) {
  // Only publish if motion state changed
  client.publish(motion_topic, motion ? "1" : "0");
  Serial.printf("Motion changed → %s\n", motion ? "Yes" : "No");
  lastMotionState = motion;
}
  }
}
