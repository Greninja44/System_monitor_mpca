#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
const char* ssid = "SafetySystem";
const char* password = "12345678";
WebServer server(80);
#define GAS_PIN     1
#define FLAME_PIN   2
#define DHT_PIN     3
#define VIB_PIN     4
#define BUZZER      5
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);
String getHTML() {
  int gas = digitalRead(GAS_PIN);
  int flame = digitalRead(FLAME_PIN);
  int vib = digitalRead(VIB_PIN);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool gasDetected = (gas == LOW);
  bool flameDetected = (flame == LOW);
  bool vibDetected = (vib == HIGH);  
  bool alert = (gasDetected || flameDetected || vibDetected || temp > 50);
  String html = "<!DOCTYPE html><html>";
  html += "<head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<style>";
  html += "body{margin:0;font-family:-apple-system,BlinkMacSystemFont,Segoe UI;background:#0f172a;color:white;}";
  html += ".header{padding:18px;text-align:center;font-size:22px;font-weight:600;background:#1e293b;}";
  html += ".container{padding:12px;}";
  html += ".card{background:#1e293b;margin:10px 0;padding:16px;border-radius:16px;";
  html += "box-shadow:0 4px 10px rgba(0,0,0,0.3);font-size:16px;}";
  html += ".title{font-size:14px;color:#94a3b8;margin-bottom:5px;}";
  html += ".value{font-size:18px;font-weight:bold;}";
  html += ".safe{border-left:6px solid #22c55e;}";
  html += ".alert{border-left:6px solid #ef4444;}";
  html += ".banner{margin:15px;padding:15px;border-radius:12px;font-weight:bold;text-align:center;}";
  html += ".banner.safe{background:#14532d;color:#4ade80;}";
  html += ".banner.alert{background:#7f1d1d;color:#f87171;animation:blink 1s infinite;}";
  html += "@keyframes blink{50%{opacity:0.6;}}";
  html += "</style></head><body>";
  html += "<div class='header'>Safety Monitoring System</div>";
  html += "<div class='container'>";
  html += "<div class='card " + String(gasDetected?"alert":"safe") + "'>";
  html += "<div class='title'>Gas Status</div>";
  html += "<div class='value'>" + String(gasDetected?"LEAK DETECTED":"Safe") + "</div>";
  html += "</div>";
  html += "<div class='card " + String(flameDetected?"alert":"safe") + "'>";
  html += "<div class='title'>Flame Status</div>";
  html += "<div class='value'>" + String(flameDetected?"FLAME DETECTED":"No Flame") + "</div>";
  html += "</div>";
  html += "<div class='card " + String(vibDetected?"alert":"safe") + "'>";
  html += "<div class='title'>Vibration</div>";
  html += "<div class='value'>" + String(vibDetected?"Detected":"Stable") + "</div>";
  html += "</div>";
  html += "<div class='card'>";
  html += "<div class='title'>Temperature</div>";
  html += "<div class='value'>" + String(temp) + " °C</div>";
  html += "</div>";
  html += "<div class='card'>";
  html += "<div class='title'>Humidity</div>";
  html += "<div class='value'>" + String(hum) + " %</div>";
  html += "</div>";
  html += "<div class='banner " + String(alert?"alert":"safe") + "'>";
  html += alert ? "WARNING: DANGER DETECTED" : "SYSTEM NORMAL";
  html += "</div>";
  html += "</div></body></html>";
  return html;
}
void handleRoot() {
  server.send(200, "text/html", getHTML());
}
void setup() {
  Serial.begin(115200);
  pinMode(GAS_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(VIB_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  dht.begin();
  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  int gas = digitalRead(GAS_PIN);
  int flame = digitalRead(FLAME_PIN);
  int vib = digitalRead(VIB_PIN);
  float temp = dht.readTemperature();
  bool gasDetected = (gas == LOW);
  bool flameDetected = (flame == LOW);
  bool vibDetected = (vib == HIGH);
  if (gasDetected || flameDetected || vibDetected || temp > 50) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}
