// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://www.shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TempSensor.h>
#include <Relay.h>
#include <IoTServer.h>
#include <IoTDevice.h>


/************************* WiFi Access Point *********************************/
#define WLAN_SSID         "myWLAN"
#define WLAN_PASS         "password"
/************************* Adafruit.io Setup *********************************/
#define MQTT_SERVER       "192.168.99.99"
#define MQTT_SERVERPORT   1883                   // use 8883 for SSL
#define MQTT_USERNAME     "mqtt-user"
#define MQTT_PW           "mqtt-pass"

#define DEVICE_REGISTRATION_TOPIC     "devmgr/device/up"
#define INIT_SUBSCRIPTION_SUFFIX      "/command/init"
#define SET_STATE_SUBSCRIPTION_SUFFIX "/command/setState"
#define RELAY_SUBSCRIPTION_SUFFIX "/command/switchRelay"



String  initSubscriptionTopic; 
String  setStateSubscriptionTopic;  
String  relaySubscriptionTopic;



int tempSensor            ;
int tempSensorPin         ;
int relay                 ;
int relayPin              ;
int inputVSensor          ;
int inputVSensorPin       ;
int publishInterval       ;
int publishIntervalSeconds;





IotDevice *g_me = NULL;
IoTServer *g_IoTServer=NULL;
TempSensor *g_tempSensor = NULL;
Relay *g_relay= NULL;





ADC_MODE(ADC_VCC);

WiFiClient net;
//MQTTClient client(1024);

unsigned long lastSensingMillis = 0;



//float senseBP(){}

float senseV(){
  return ESP.getVcc()/1000.00;
}

float senseI(){
  return 0;
}

float senseL(){
  return 0;
}


bool senseAndReportAll(){
  
  bool result=true;
  
  if(tempSensor !=0){
    g_IoTServer->reportMetric("T", g_tempSensor->getT());
    float Rh=g_tempSensor->getRh();
    if(Rh>=0) g_IoTServer->reportMetric("Rh", Rh);
  }
  g_IoTServer->reportMetric("inputV", senseV());
  
  
  //todo: add more sensors
  return result;  
}    

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!g_IoTServer->connect(g_me->id().c_str(), MQTT_USERNAME, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");

  g_IoTServer->subscribe(initSubscriptionTopic);
  g_IoTServer->subscribe(setStateSubscriptionTopic);
  g_IoTServer->subscribe(relaySubscriptionTopic);
  // client.unsubscribe("/hello");



  
}

void tempSensorSetup(int tempSensor, int tempSensorPin){
   switch(tempSensor) {
   case 0  : //no sensor
      return;
   case 1  : //DHT11
      g_tempSensor= new DHT11TempSensor(tempSensorPin);
      break; 
   case 2  : //DHT22
      g_tempSensor= new DHT22TempSensor(tempSensorPin);
      break; 
   case 3  : //Dallas
      g_tempSensor= new DallasTempSensor(tempSensorPin);
      break; 
   }
}

void relaySetup(int relay, int relayPin){
  if(relay>0){
    g_relay= new Relay(relayPin);
  }
};

void inputVSensorSetup(int inputVSensor,int inputVSensorPin){};

void initDevice(String &payload){
  Serial.println("init");

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  
  const char* tmp=doc["device_name"];
  g_me->setName(tmp);
  g_IoTServer->setName(tmp);
  Serial.println(g_me->name());
  
  tempSensor              = doc[ "temp_sensor"               ];
  tempSensorPin           = doc[ "temp_sensor_pin"           ];
  relay                   = doc[ "relay"                     ];
  relayPin                = doc[ "relay_pin"                 ];
  inputVSensor            = doc[ "input_v_sensor"            ];
  inputVSensorPin         = doc[ "input_v_sensor_pin"        ]; 
  publishInterval         = doc[ "publish_interval"          ]; //millis
  publishIntervalSeconds  = doc[ "publish_interval_seconds"  ]; //seconds

  tempSensorSetup(tempSensor, tempSensorPin);
  relaySetup(relay, relayPin);
  inputVSensorSetup(inputVSensor,inputVSensorPin);
   relaySubscriptionTopic    =g_me->name() +RELAY_SUBSCRIPTION_SUFFIX;
 

    g_IoTServer->subscribe(relaySubscriptionTopic);


  g_me->initialize();  
}

void setState(String &payload){
  Serial.println("setState: "+ payload);
}

void manageRelay(String &payload){
  Serial.println("manageRelay: "+ payload);
  String command=payload;
  command.toUpperCase();

  if(command=="ON"){
    g_IoTServer->reportStatus("relay", g_relay->switchOn());
  } else if (command=="OFF"){
    g_IoTServer->reportStatus("relay", g_relay->switchOff());
  } else{
    g_IoTServer->reportStatus("relay", g_relay->toggle());
  }
};

void messageReceived(String &topic, String &payload) {
  
  Serial.println("incoming: " + topic + " - " + payload);
  
  if ( topic.equals( initSubscriptionTopic     ) ) initDevice ( payload );
  if ( topic.equals( setStateSubscriptionTopic ) ) setState   ( payload );
  if ( topic.equals( relaySubscriptionTopic    ) ) manageRelay( payload );

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

String getDeviceId(){
    String deviceId = WiFi.macAddress();
    deviceId.replace(':','_');
    return deviceId;
  }

void commInit(){
  Serial.begin(115200);
  delay(5000);
  Serial.println("starting");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.println("wifi.begin");
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  delay(5000);
  Serial.println(WiFi.localIP());  
  Serial.println(getDeviceId());
}

void setup() {
  
  commInit();

  g_me = new IotDevice(getDeviceId()) ;
  Serial.println("calling iots + " + g_me->name());
  g_IoTServer=new IoTServer(MQTT_SERVER,g_me->name(), net, messageReceived);
  

  initSubscriptionTopic        =g_me->id() +INIT_SUBSCRIPTION_SUFFIX;
  setStateSubscriptionTopic    =g_me->id() +SET_STATE_SUBSCRIPTION_SUFFIX;
  
  
  connect();
    
  long lastInitTryMillis=0;
  
  while (!g_me->bInitialized()) {
    if (millis() - lastInitTryMillis > 60000) {
      lastInitTryMillis = millis();
      g_IoTServer->publish(DEVICE_REGISTRATION_TOPIC     , g_me->id());
      Serial.println (DEVICE_REGISTRATION_TOPIC);
      Serial.println(g_me->id());
    }
    g_IoTServer->loop();
    delay(120);
    Serial.print(F("."));
  }
  Serial.print(F("DeviceName"));
  Serial.println(g_me->name());
  


  
}

void loop() {
  
  g_IoTServer->loop();
  delay(10);  // <- fixes some issues with WiFi stability
  
  if(g_IoTServer->lastError())
    Serial.println(g_IoTServer->lastError());
 
  if (publishIntervalSeconds<=0) {
    Serial.println(F("No publish interval set!"));
    //retry in a minute...
  
  } 
  else if( millis() - lastSensingMillis > publishIntervalSeconds * 1000 ) {
    lastSensingMillis = millis();
    senseAndReportAll();
    //Serial.print(".");
  } 
  
  if (!g_IoTServer->connected()) {
    connect();
  }
 
}