#define BLYNK_TEMPLATE_ID "TMPL3wyxXKG8m"
#define BLYNK_TEMPLATE_NAME "Alert"
#define BLYNK_AUTH_TOKEN "8p4MxEb4YKpQ7k5MDAthF-4aZfPJxRIA"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TinyGPS++.h>
#include <ESP8266WebServer.h>

// Define pins
#define TRIGGER_PIN D1
#define ECHO_PIN    D2
#define LED_PIN     D5
#define GPS_RX_PIN  15  // GPIO15 (D8)
#define GPS_TX_PIN  13  // GPIO13 (D7)
#define BUTTON_PIN  D3  // GPIO0 (D3) - Button input pin
#define BUZZER_PIN  D4  // GPIO2 (D4) - Buzzer output pin
#define EMERGENCY_BUTTON_PIN D6  // GPIO12 (D6) - Emergency Button
#define BUZZER2_PIN D0  // GPIO13 (D0) - Second Buzzer output pin

// Authentication and network credentials
char auth[] = "8p4MxEb4YKpQ7k5MDAthF-4aZfPJxRIA";
char ssid[] = "wifi";
char pass[] = "8970834835**";

// Global variables
unsigned long distance;
TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);

// Debounce variables for emergency button
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool emergencyAlert = false;

// Web server
ESP8266WebServer server(80);

// Function to handle the root web page
void handleRoot() {
    String message = "<h1>Rescue Aid System</h1>";
    message += "<style>";
    message += "/* Water background animation */";
    message += "body {";
    message += "    margin: 0;";
    message += "    padding: 20px;";
    message += "    min-height: 100vh;";
    message += "    background: linear-gradient(180deg, #87CEEB 0%, #1E90FF 100%);";
    message += "    font-family: Arial, sans-serif;";
    message += "    color: white;";
    message += "}";
    
    message += "/* Water wave animation */";
    message += "@keyframes wave {";
    message += "    0% { transform: translateY(0) rotate(0deg); }";
    message += "    50% { transform: translateY(-10px) rotate(5deg); }";
    message += "    100% { transform: translateY(0) rotate(0deg); }";
    message += "}";
    
    message += "/* Header styling */";
    message += "h1 {";
    message += "    text-align: center;";
    message += "    color: white;";
    message += "    text-shadow: 2px 2px 4px rgba(0,0,0,0.3);";
    message += "    margin-bottom: 30px;";
    message += "}";
    
    message += "/* Data table styling */";
    message += ".data-table {";
    message += "    background: rgba(255, 255, 255, 0.9);";
    message += "    border-radius: 10px;";
    message += "    padding: 20px;";
    message += "    box-shadow: 0 4px 6px rgba(0,0,0,0.1);";
    message += "    margin: 20px 0;";
    message += "    width: 100%;";
    message += "    max-width: 600px;";
    message += "    margin-left: auto;";
    message += "    margin-right: auto;";
    message += "}";
    
    message += "/* Table text color */";
    message += ".data-table th {";
    message += "    background-color: #1E90FF;";
    message += "    color: white;";
    message += "    padding: 12px;";
    message += "    text-align: left;";
    message += "    border-bottom: 1px solid #ddd;";
    message += "    border-radius: 5px 5px 0 0;";
    message += "}";
    
    message += ".data-table td {";
    message += "    color: #333;";
    message += "    padding: 12px;";
    message += "    text-align: left;";
    message += "    border-bottom: 1px solid #ddd;";
    message += "}";
    
    message += "/* Emergency button styling */";
    message += ".emergency-button {";
    message += "    display: block;";
    message += "    margin: 20px auto;";
    message += "    padding: 15px 30px;";
    message += "    background-color: #ff4444;";
    message += "    color: white;";
    message += "    border: none;";
    message += "    border-radius: 25px;";
    message += "    cursor: pointer;";
    message += "    font-size: 18px;";
    message += "    transition: all 0.3s ease;";
    message += "    box-shadow: 0 4px 8px rgba(0,0,0,0.2);";
    message += "}";
    
    message += "/* Boat animation */";
    message += ".boat {";
    message += "    position: absolute;";
    message += "    bottom: 0;";
    message += "    left: 50%;";
    message += "    transform: translateX(-50%);";
    message += "    width: 60px;";
    message += "    height: 40px;";
    message += "    background: white;";
    message += "    clip-path: polygon(0 50%, 100% 0, 100% 100%);";
    message += "    animation: wave 3s ease-in-out infinite;";
    message += "}";
    
    message += "/* Responsive design */";
    message += "@media (max-width: 600px) {";
    message += "    .data-table {";
    message += "        margin: 10px;";
    message += "        padding: 10px;";
    message += "    }";
    
    message += "    .emergency-button {";
    message += "        width: 90%;";
    message += "        padding: 12px 25px;";
    message += "    }";
    message += "}";
    message += "</style>";
    
    message += "<div class='boat'></div>";
    message += "<table class='data-table'>";
    message += "<tr><th>Parameter</th><th>Value</th></tr>";
    message += "<tr><td>Distance</td><td id='distance'>" + String(distance) + " cm</td></tr>";
    
    if (gps.location.isValid()) {
        message += "<tr><td>Latitude</td><td>" + String(gps.location.lat(), 6) + "</td></tr>";
        message += "<tr><td>Longitude</td><td>" + String(gps.location.lng(), 6) + "</td></tr>";
    } else {
        message += "<tr><td>GPS Status</td><td>Not available</td></tr>";
    }
    message += "</table>";
    
    if (emergencyAlert) {
        message += "<p style='color:red; text-align: center;'>Emergency Alert Triggered!</p>";
    }
    
    message += "<p style='text-align: center;'><a href='/emergency' class='emergency-button'>Trigger Emergency Alert</a></p>";
    
    // Add JavaScript for AJAX
    message += "<script>";
    message += "function updateDistance() {";
    message += "    var xhr = new XMLHttpRequest();";
    message += "    xhr.onreadystatechange = function() {";
    message += "        if (this.readyState == 4 && this.status == 200) {";
    message += "            document.getElementById('distance').innerHTML = this.responseText + ' cm';";
    message += "        }";
    message += "    };";
    message += "    xhr.open('GET', '/getDistance', true);";
    message += "    xhr.send();";
    message += "}";
    message += "setInterval(updateDistance, 1000);";  // Update every 1 second
    message += "</script>";
    
    server.send(200, "text/html", message);
}

// Function to handle distance updates via AJAX
void handleGetDistance() {
    server.send(200, "text/plain", String(distance));
}

// Function to handle emergency button press
void handleEmergency() {
    emergencyAlert = true;
    if (Blynk.connected()) {
        Blynk.logEvent("danger_alert", "Emergency Button Pressed!");
        Serial.println("Emergency Alert Sent!");
    } else {
        Serial.println("Blynk Not Connected! Alert Failed.");
    }
    server.send(200, "text/html", "Emergency alert triggered!");
}

// Function to reset emergency alert
void handleResetEmergency() {
    emergencyAlert = false;
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", "Emergency alert reset!");
}

void setup() {
    Serial.begin(9600);
    GPS_Serial.begin(9600, SERIAL_8N1, (SerialMode)GPS_RX_PIN, GPS_TX_PIN, SERIAL_FULL);
    
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(EMERGENCY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER2_PIN, OUTPUT);
    
    Blynk.begin(auth, ssid, pass);
    WiFi.begin(ssid, pass);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    server.on("/", handleRoot);
    server.on("/getDistance", handleGetDistance);  // Register the new handler
    server.on("/emergency", handleEmergency);
    server.on("/reset-emergency", handleResetEmergency);
    server.begin();
    Serial.println("Web server started");
}

void loop() {
    Blynk.run();
    server.handleClient();
    
    while (GPS_Serial.available()) {
        gps.encode(GPS_Serial.read());
    }
    
    long duration;
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(8);
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) / 29.1;
    
    Blynk.virtualWrite(V1, distance);
    
    if (gps.location.isValid()) {
        Blynk.virtualWrite(V2, gps.location.lat(), 6);
        Blynk.virtualWrite(V3, gps.location.lng(), 6);
        Serial.println("GPS Found!");
        Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
    } else {
        Serial.println("No GPS Found!");
    }
    
    Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");
    
    if (distance <= 10 && distance > 0) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
    
    if (digitalRead(BUTTON_PIN) == HIGH) {
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("Button pressed - Buzzer ON");
    } else {
        digitalWrite(BUZZER_PIN, HIGH);
        Serial.println("Button released - Buzzer OFF");
    }
    
    int reading = digitalRead(EMERGENCY_BUTTON_PIN);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading == LOW && !buttonPressed) {
            buttonPressed = true;
            emergencyAlert = true;
            Serial.println("Emergency Button Pressed! Sending Alert...");
            if (Blynk.connected()) {
                Blynk.logEvent("danger_alert", "Emergency Button Pressed!");
                Serial.println("Alert Sent!");
            } else {
                Serial.println("Blynk Not Connected! Alert Failed.");
            }
        } else if (reading == HIGH) {
            buttonPressed = false;
        }
    }
    
    lastButtonState = reading;
    
    if (emergencyAlert) {
        static unsigned long lastBlink = 0;
        if (millis() - lastBlink > 500) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            lastBlink = millis();
        }
    }
}

BLYNK_WRITE(V4) {
    int buzzer2State = param.asInt();
    if (buzzer2State == 1) {
        digitalWrite(BUZZER2_PIN, HIGH);
    } else {
        digitalWrite(BUZZER2_PIN, LOW);
    }
}