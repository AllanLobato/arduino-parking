#include <LiquidCrystal_I2C.h>
#include "EspMQTTClient.h"


const int trigPin = 5;
const int echoPin = 18;
int lcdColumns = 16;
int lcdRows = 2;

//define sound speed in cm/uS

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

EspMQTTClient client(
  "Wokwi-GUEST",         // SSID WiFi
  "",                    // Password WiFi
  //"test.mosquitto.org",  // MQTT Broker
  "broker.hivemq.com",  // MQTT Broker
  "mqtt-mack",          // Client
  1883                   // MQTT port
);

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(19, OUTPUT);
  pinMode(23, OUTPUT);
  lcd.init();
  lcd.clear();
  lcd.backlight();

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
  
  
}

void lerEnviarDados() {
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  client.publish("mackenzie20507429/ocupado", String(distanceCm) + " cm");
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
if (distanceCm <= 50) {
    lcd.setCursor(0, 0);
    lcd.print("Vaga Ocupada!");
    digitalWrite(19, HIGH);
    digitalWrite(23, LOW);


  } else {
    lcd.setCursor(0,0);
    lcd.print("Vaga Livre :)");
    digitalWrite(23, HIGH);
    digitalWrite(19, LOW);
    // client.publish("mackenzie20507429/livre", String(distanceCm) + "Vaga Livre :)");
  }

  // Serial.print("Distance (cm): ");
  // Serial.println(distanceCm);
  // Serial.print("Distance (inch): ");
  // Serial.println(distanceInch);
}

void onConnectionEstablished()
{
  // Subscribe no "topicowokwi/msgRecebida/#" e mostra a mensagem recebida na Serial
    client.subscribe("mackenzie20507429/msgRecebida/#", [](const String & topic, const String & payload) {
    Serial.println("Mensagem recebida no topic: " + topic + ", payload: " + payload);
  });

  lerEnviarDados();
}


void loop()
{
  client.loop(); // Executa em loop
  lerEnviarDados();
  delay(1000);
}