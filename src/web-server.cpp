#include <WiFiS3.h>
#include "arduino_secrets.h"
#include "Arduino_LED_Matrix.h"
#include "animations/hi.h"
#include "animations/bye.h"


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

WiFiServer server(80);  // Create a server that listens on port 80

void printMacAddress(byte mac[]);
String parseRoute(String request); 

ArduinoLEDMatrix matrix;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection:
    delay(10000);
  }

  // You're connected now, so print out the data:
  Serial.println("You're connected to the network");
  // printCurrentNet();
  // printWifiData();

  // Start the server
  server.begin();
  Serial.println("👍 Server started 🚀");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    String currentLine = "";  // Make a String to hold incoming data from the client
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        currentLine += c; 

        // Check if the request header has ended (double newline)
        if (currentLine.endsWith("\r\n\r\n")) {
          // Extract the route from the request
          String route = parseRoute(currentLine);
          Serial.print("Requested route: ");
          Serial.println(route);

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
          if (route.equals("/hello")) {
            client.println("{\"message\": \"Hello!\"}");
            matrix.loadSequence(hi);
            matrix.begin();
            matrix.play(true);
          } else if (route.equals("/goodbye")) {
            client.println("{\"message\": \"Goodbye!\"}");
            matrix.loadSequence(bye);
            matrix.begin();
            matrix.play(true);
          } else {
            client.println("{\"message\": \"Route not found!\"}");
          }
          client.println();
          client.stop();
          Serial.println("Client disconnected");
          break; 
      }
    }
  }
}
}

String parseRoute(String request) {
  // Find the first space after "GET "
  int start = request.indexOf("GET ") + 4;
  // Find the next space after "GET "
  int end = request.indexOf(" ", start);
  // Extract the substring between the spaces, which should be the route
  return request.substring(start, end);
}


void printWifiData() {
  // Print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // Print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.println(rssi);

  // Print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}
