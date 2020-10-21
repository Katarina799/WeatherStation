#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "PENTHOUSE";
const char* password = "hasloDoInternetu2019";
const char* mqtt_server = "192.168.0.103";
const char* deviceName = "RaspberryPi";

const char* startMessageTopic = "esp32/message";
const char* pinTopic = "esp32/gpio";
const int ledPin = 5;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect()
{
  bool ctd = false;
  Serial.println("Rozlaczono!\n");
  while(!ctd)
  {
    Serial.print("Laczenie z serwerem...\n");
    if(client.connect(deviceName))
    {
      ctd = true;
      Serial.println("Polaczono!\n");
    }
    else
    {
      Serial.print(".");
      delay(1000);
    }
  }
}

void odbiorWiadomosci(String temat, byte* zawartosc, unsigned int dlugosc)
{
  String pomoc;
  Serial.println("Odebrano wiadomosc:");
  Serial.print("\tTemat: ");
  Serial.println(temat);
  Serial.print("\tTresc: \"");
  for(int i=0; i<dlugosc; i++) 
  {
    Serial.print((char)zawartosc[i]);
    pomoc += (char)zawartosc[i];
  }
  Serial.println("\"");

  if(temat == pinTopic)
  {
    if(pomoc == "1")
    {
     
      digitalWrite(ledPin, HIGH);
      Serial.println("LED1: ON"); 
    }
    else if(pomoc == "0")
    {
      digitalWrite(ledPin, LOW);
      Serial.println("LED1: OFF");
    }
    else
      Serial.println("Nieznana komenda, nie wiem co mam z tym zrobic");
  }
}

void ustawienieWifi()
{
  delay(10);
  Serial.println();
  Serial.print("Laczenie z ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.setHostname("ESP32");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Polaczona z wifi.\nESP otrzymalo adres IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin,OUTPUT);
  ustawienieWifi();
  delay(1000);
  client.setServer(mqtt_server, 1883);
  client.connect(deviceName);
  client.subscribe(pinTopic);
  client.setCallback(odbiorWiadomosci);
  client.publish(startMessageTopic, "Hello from ESP32");
}

void loop() {
  
  if (!client.connected())
  {
   reconnect();
   client.publish(startMessageTopic, "Hello from ESP32");
  }
  if(!client.loop())
  {
    client.connect(deviceName);
  } 
  
}
