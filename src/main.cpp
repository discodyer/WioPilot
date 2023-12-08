#include "TFT_eSPI.h" //include TFT LCD library
#include <rpcWiFi.h>
#include <WiFiUdp.h>

TFT_eSPI tft; // initialize TFT LCD

TFT_eSprite spr = TFT_eSprite(&tft); // initialize buffer

void connectToWiFi(const char *ssid, const char *pwd);
void sendPackage(const char *message);
void WiFiEvent(WiFiEvent_t event);

// WiFi network name and password:
const char *networkName = "your-ssid";
const char *networkPswd = "your-passwd";

// IP address to send UDP data to:
//  either use the ip address of the server or
//  a network broadcast address
const char *udpAddress = "192.168.0.0";
const int udpPort = 3333;

// Are we currently connected?
boolean connected = false;

// The udp library class
WiFiUDP udp;

boolean is_send = true;
String pkg_to_send;

void button_handler_up() { sendPackage("button Up"); }
void button_handler_down() { sendPackage("button Down"); }
void button_handler_left() { sendPackage("button left"); }
void button_handler_right() { sendPackage("button Right"); }
void button_handler_press() { sendPackage("button Press"); }

void setup()
{

  // Initilize hardware serial:
  Serial.begin(115200);

  // Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

  /* Button setup */
  pinMode(WIO_5S_UP, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);
  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT, INPUT);
  pinMode(WIO_5S_PRESS, INPUT);

  /* Attach */
  attachInterrupt(digitalPinToInterrupt(WIO_5S_UP), button_handler_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_DOWN), button_handler_down, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_LEFT), button_handler_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_RIGHT), button_handler_right, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_PRESS), button_handler_press, FALLING);
}
void loop()
{
  if (!is_send)
  { // only send data when connected
    is_send = true;
    if (connected)
    {
      // Send a packet
      udp.beginPacket(udpAddress, udpPort);
      udp.printf(pkg_to_send.c_str());
      udp.endPacket();
    }
    // Wait for 1 second
  }
  delay(200);
}
void connectToWiFi(const char *ssid, const char *pwd)
{
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  // register event handler
  WiFi.onEvent(WiFiEvent);

  // Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

// wifi event handler
void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    // When connected set
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    // initializes the UDP state
    // This initializes the transfer buffer
    udp.begin(WiFi.localIP(), udpPort);
    connected = true;
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    connected = false;
    break;
  default:
    break;
  }
}
void sendPackage(const char *message)
{
  // if (connected)
  // {
  //   // Send a packet
  //   udp.beginPacket(udpAddress, udpPort);
  //   udp.printf(message);
  //   Serial.println(message);
  //   udp.endPacket();
  // }
  // delay(1000);
  pkg_to_send = message;
  Serial.println(message);
  is_send = false;
}
