#include "WiFi.h"
#include <HTTPClient.h>
#include "DHT.h"
#include "WiFiClientSecure.h"
#include "Fuzzy.h"
#include <WebSocketsServer.h>
#include <map>
#include <Preferences.h>
#include <WebServer.h>

#define DHTTYPE DHT22

const char* BOT_TOKEN = "";
const char* ChannelID = "@fire_test10";
const float VCC = 3.3;
const String SecretKey = "rahasia";

const int BUZZER = 4;
const int fireSensor = 35;
const int gasSensor = 34;
const int tempSensor = 19;
const int LED = 18;

float calibratedRo;
bool buzzerState = false;
bool wifiConnected = false;
//interval buat aktif pasca kebaakaran selam 30mnt
const unsigned long fireInterval = 1000 * 60 * 30;
//nampung kpn terakhir kali kebakaran
unsigned long lastFire = 0;
//nampung kapan terkhir kali kirim ke tele
unsigned long lastSended = 0;
//interval biar ga terus terusan ngiriim ke tele
const unsigned long interval = 10000;

WebSocketsServer ws(80);
DHT dht(tempSensor, DHTTYPE);
WebServer httpServer(8000);
Preferences pref;



struct DhtData {
  float temperature;
  float humidity;
};

struct FuzzyData {
  float fire;
  int gas;
  DhtData dht;
};

std::map<uint8_t, bool> clients;


FuzzyData getFuzzy();
DhtData readDht();
String toString(const FuzzyData& data);
void sendToTelegram(void* param);
bool getTokenFromURL(const String& url);
void onWebSocketEvent(uint8_t fd, WStype_t type, uint8_t* payload, size_t length);
float calibrateRo(int gasSensorPin);
void wsTask(void* param);
void mainTask();
void httpServerSetup();
void connectionChecker();



void setup() {
  String storedSsid, storedPass;
  pref.begin("wifi", true);
  storedSsid = pref.getString("ssid", ".");
  storedPass = pref.getString("pass", "aicomel06");
  pref.end();
  dht.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.setHostname("Esp32FireDetector");
  IPAddress local_ip(192, 168, 10, 1);
  IPAddress gateway(192, 168, 10, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP("fireDetector", "password123");

  WiFi.begin(storedSsid.c_str(), storedPass.c_str());

  pinMode(fireSensor, INPUT);
  pinMode(gasSensor, INPUT_PULLDOWN);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, LOW);

  Serial.println("Preheating MQ-2");
  delay(20000);
  Serial.println("Calibrating RO");

  float roSum = 0;
  //kalibrasi otomatis
  for (int i = 0; i < 50; i++) {
    float tempRo = calibrateRo(gasSensor);
    Serial.print("RO: ");
    Serial.println(tempRo);
    roSum += tempRo;
    delay(100);
  }
  calibratedRo = roSum / 50.0;
  Serial.print("Calibrated Ro: ");
  Serial.println(calibratedRo);

  Serial.println("Connecting to WiFi...");
  unsigned long start = millis();
  unsigned long waitingTime = 10000;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (start - millis() >= waitingTime) {
      break;
    }
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
  }
  httpServerSetup();
  ws.begin();
  ws.onEvent(onWebSocketEvent);
  xTaskCreatePinnedToCore(wsTask, "WebSocketLoop", 8192, NULL, 1, NULL, 1);
}

void wsTask(void* param) {
  while (1) {
    ws.loop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


void loop() {
  httpServer.handleClient();
  connectionChecker();
  if (wifiConnected) {
    mainTask();
  }
  delay(500);
}

bool mayBeStillOnFire() {
  return lastFire != 0 ? millis() - lastFire <= fireInterval : false;
}


FuzzyData getFuzzy() {
  FuzzyData data;
  data.fire = analogRead(fireSensor) * (3.3 / 4095.0);
  data.gas = analogRead(gasSensor);
  data.dht = readDht();
  return data;
}

DhtData readDht() {
  DhtData d;
  d.humidity = dht.readHumidity();
  d.temperature = dht.readTemperature();
  return d;
}

String toString(const FuzzyData& data) {
  return "Fire: " + String(data.fire) + ", " + String(data.gas) + ", Temp: " + String(data.dht.temperature) + "°C, Humidity: " + String(data.dht.humidity) + "%";
}

float calibrateRo(int gasSensorPin) {
  float adc = analogRead(gasSensorPin);
  float vout = (adc * (3.3 / 4095.0));
  float rs = 10.0 * ((3.3 / vout) - 1);
  float ro = rs / 9.9;
  return ro;
}

void sendToTelegram(void* param) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Skipping Telegram send.");
    vTaskDelete(NULL);
    return;
  }

  WiFiClientSecure secure;
  secure.setInsecure();

  HTTPClient client;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) + "/sendMessage?chat_id=" + String(ChannelID) + "&text=Fire detected";
  client.begin(secure, url);

  int code = client.GET();
  if (code > 0) {
    Serial.println("Message Sent! Code: " + String(code));
  } else {
    Serial.println("Failed to send message. Error: " + client.errorToString(code));
  }
  client.end();
  vTaskDelete(NULL);
}

bool getTokenFromURL(const String& url) {
  if (url.startsWith("/?token=")) {
    int start = 8;
    int end = url.indexOf('&', start);
    String token = (end == -1) ? url.substring(start) : url.substring(start, end);
    return token == SecretKey;
  }
  return false;
}

void onWebSocketEvent(uint8_t fd, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      {
        String url = String((char*)payload);
        bool is_esp = getTokenFromURL(url);
        clients[fd] = is_esp;

        bool fire = mayBeStillOnFire();
        if (is_esp) {
          ws.sendTXT(fd, fire ? "on" : "off");
        } else {
          ws.sendTXT(fd, fire ? "on" : "off");
        }
        break;
      }
    case WStype_DISCONNECTED:
      {
        Serial.printf("[WS] Client #%u disconnected\n", fd);
        clients.erase(fd);
        break;
      }
    case WStype_TEXT:
      {
        String msg = String((char*)payload);

        if (msg == "status") {
          bool fire = mayBeStillOnFire();
          ws.sendTXT(fd, fire ? "on" : "off");
        }

        else {
          if (clients[fd]) {
            for (const auto& pair : clients) {
              if (!pair.second) {
                ws.sendTXT(pair.first, msg);
                delay(50);
              }
            }
          }
        }
        break;
      }
  }
}

void mainTask() {
  FuzzyData x = getFuzzy();
  Fuzzy fuzzy(x.fire, x.dht.temperature, x.gas, calibratedRo);
  Serial.println(toString(x));
  Serial.print("PPM : ");
  Serial.println(fuzzy.getPpm());
  Fuzzy def = fuzzy.defuzzification();
  if (def.shouldWarn()) {
    lastFire = millis();
    buzzerState = true;
    if (millis() - lastSended >= interval) {

      lastSended = millis();
      xTaskCreatePinnedToCore(sendToTelegram, "announcument", 4096, NULL, 1, NULL, 1);
      ws.broadcastTXT("on");
    } else {
      Serial.println(millis() - lastSended);
    }
    Serial.println("Warning! fire detected");
  } else {
    buzzerState = false;
    if (millis() - lastFire >= fireInterval) {
      ws.broadcastTXT("off");
    }
  }
  digitalWrite(LED, buzzerState ? HIGH : LOW);
  digitalWrite(BUZZER, buzzerState ? LOW : HIGH);
}


void httpServerSetup() {
  httpServer.on("/", HTTP_GET, []() {
    String html = "<h1>Connection Status : " + String(wifiConnected ? "connected" : "disconnected") + "</h1>";
    html += "<form action='/save' method='POST'>SSID: <input name='ssid'><br>Password: <input name='pass' type='password'><br><button type='submit'>Save</button></form>";
    httpServer.send(200, "text/html",html);
  });
  httpServer.on("/save", HTTP_POST, []() {
    if (httpServer.hasArg("ssid") && httpServer.hasArg("pass")) {
      String nssid = httpServer.arg("ssid");
      String npass = httpServer.arg("pass");
      pref.begin("wifi", false);
      pref.putString("ssid", nssid);
      pref.putString("pass", npass);
      pref.end();
      httpServer.send(200);
      ESP.restart();

    } else {
      httpServer.send(400, "application/json", "{\"message\":\"missing pass or ssid\"}");
    }
  });
  httpServer.begin();
}


void connectionChecker() {
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
  } else {
    wifiConnected = false;
  }
}