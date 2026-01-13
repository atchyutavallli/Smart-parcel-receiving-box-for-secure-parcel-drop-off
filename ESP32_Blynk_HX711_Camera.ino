define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3Xm8PIESG"
#defineBLYNK_TEMPLATE_NAME "controlsolenoid door lock"
#define BLYNK_AUTH_TOKEN "Grm52pBhfLdxM-YZOJMv-6fi2Dbax3G-" 
#include <WiFi.h>
#include <BlynkSimpleEsp32.h> 
#include "HX711.h"
// HX711 pins 
#define DOUT 21
#define SCK 22 
HX711 scale;
// Relay pin
#define RELAY_PIN 2
// Calibration factor for 10kg load cell
float calibration_factor = 20820.0; // update after calibration
// Blynk auth token
// WiFi credentials
char ssid[] = "ARUN. 7"; 
char pass[] = "arun19915";
// Virtual pin for weight display 
#define VPIN_WEIGHT V1
// Virtual pin for relay control 
#define VPIN_RELAY V2 
void setup() { 
Serial.begin(115200);
// Blynk
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
// HX711 setup 
scale.begin(DOUT, SCK); 
scale.set_scale(calibration_factor); 
scale.tare(); // zero with no weight
// Relay pin setup 
pinMode(RELAY_PIN, OUTPUT);
digitalWrite(RELAY_PIN, LOW); // Relay off initially 
Serial.println("ESP32 + HX711 + Relay + Blynk Ready");
}
// Relay control from Blynk button 
BLYNK_WRITE(VPIN_RELAY){
int relayState = param.asInt(); // 1 or 0 
digitalWrite(RELAY_PIN, relayState); 
Serial.print("Relay set to: ");
Serial.println(relayState);
}
void loop() { 
Blynk.run();
// Read weight
float weight = scale.get_units(10); // average 10 readings 
Serial.print("Weight: ");
Serial.println(weight, 2);
//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
// Send weight to Blynk app 
Blynk.virtualWrite(VPIN_WEIGHT, weight);
delay(1000);
}
#include "esp_camera.h" 
#include <WiFi.h>
// WiFi credentials
const char* ssid = "Arun. 7";
const char* password = "arun19915";
// AI Thinker ESP32-CAM Pin Configuration 
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM
0
#define SIOD_GPIO_NUM
26
#define SIOC_GPIO_NUM
27
#define Y9_GPIO_NUM
35
#define Y8_GPIO_NUM
34
#define Y7_GPIO_NUM
39
#define Y6_GPIO_NUM
36
#define Y5_GPIO_NUM
21
#define Y4_GPIO_NUM
19
#define Y3_GPIO_NUM
18
#define Y2_GPIO_NUM
5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM
23
#define PCLK_GPIO_NUM
22
#define LED_GPIO_NUM
4 // Flash LED pin
// Web server for streaming
#include <WebServer.h> 
WebServer server(80);
void setFlashlight(int brightness) {
ledcSetup(1, 5000, 8);
// Channel 1, 5KHz, 8-bit resolution
ledcAttachPin(LED_GPIO_NUM, 1); 
ledcWrite(1, brightness); // 0-255 brightness
}
void handleFlash() {
if (server.hasArg("brightness")) {
int brightness = server.arg("brightness").toInt(); 
setFlashlight(brightness);
server.send(200, "text/plain", "OK");
} else {
server.send(400, "text/plain", "Bad Request");
}
}
void handle_jpg_stream() { 
WiFiClient client = server.client();
String response = "HTTP/1.1 200 OK\r\n";
response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n"; 
server.sendContent(response);
while (true) {
camera_fb_t *fb = esp_camera_fb_get(); 
if (!fb) {
Serial.println("Camera capture failed"); 
break;
}
response = "--frame\r\nContent-Type: image/jpeg\r\n\r\n"; 
server.sendContent(response);
server.sendContent((const char *)fb->buf, fb->len); 
server.sendContent("\r\n"); 
esp_camera_fb_return(fb);
if (!client.connected()) break; 
delay(10);
}
}
void startCameraServer() { 
server.on("/", []() {
String html = "<html><body>";
html += "<h1>ESP32-CAM Stream</h1>";
html += "<img src='/stream' width='640' height='480'>"; 
html += "<br><br>";
html += "<h2>Flashlight Control</h2>";
html += "<input type='range' min='0' max='255' value='0' id='brightnessSlider' 
oninput='setBrightness(this.value)'>";
html += "<script>";
html += "function setBrightness(value) {"; 
html += " fetch('/flash?brightness=' + value);"; 
html += "}";
html += "</script>";
html += "</body></html>"; 
server.send(200, "text/html", html);
});
server.on("/stream", HTTP_GET, handle_jpg_stream); 
server.on("/flash", HTTP_GET, handleFlash); // Flash control endpoint 
server.begin();
}
void setup() { 
Serial.begin(115200); 
Serial.setDebugOutput(true);
Serial.println("\nStarting ESP32-CAM");
// Camera configuration 
camera_config_t config;
config.ledc_channel = LEDC_CHANNEL_0; 
config.ledc_timer = LEDC_TIMER_0; 
config.pin_d0 = Y2_GPIO_NUM; 
config.pin_d1 = Y3_GPIO_NUM; 
config.pin_d2 = Y4_GPIO_NUM; 
config.pin_d3 = Y5_GPIO_NUM; 
config.pin_d4 = Y6_GPIO_NUM; 
config.pin_d5 = Y7_GPIO_NUM; 
config.pin_d6 = Y8_GPIO_NUM; 
config.pin_d7 = Y9_GPIO_NUM; 
config.pin_xclk = XCLK_GPIO_NUM; 
config.pin_pclk = PCLK_GPIO_NUM; 
config.pin_vsync = VSYNC_GPIO_NUM; 
config.pin_href = HREF_GPIO_NUM; 
config.pin_sscb_sda = SIOD_GPIO_NUM; 
config.pin_sscb_scl = SIOC_GPIO_NUM; 
config.pin_pwdn = PWDN_GPIO_NUM; 
config.pin_reset = RESET_GPIO_NUM; 
config.xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_RGB565; 
config.frame_size = FRAMESIZE_QVGA; 
config.jpeg_quality = 10;
config.fb_count = 1;
// Initialize camera
esp_err_t err = esp_camera_init(&config); 
if (err != ESP_OK) {
Serial.printf("Camera init failed: 0x%x\n", err); 
delay(1000);
ESP.restart(); 
return;
}
Serial.println("Camera initialized");
// Initialize LED flash (PWM) 
pinMode(LED_GPIO_NUM, OUTPUT);
setFlashlight(0); // Start with LED off
// Connect to WiFi 
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) { 
delay(500);
Serial.print(".");
}
Serial.println("\nWiFi connected"); 
Serial.print("Camera Stream URL: http://"); 
Serial.println(WiFi.localIP());
startCameraServer();
}
void loop() { 
server.handleClient(); 
delay(2);