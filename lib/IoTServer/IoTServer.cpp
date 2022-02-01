#include <ESP8266WiFi.h>
#include <MQTT.h>

class IoTServer{
    public:
        IoTServer(String server, String clientName, Client &net, MQTTClientCallbackSimple callback){

            clientName_=clientName;
            Serial.print("creating IoTServer "+ clientName_ );
            client.begin(server.c_str(), net);
            client.onMessage(callback);
        }
        
        void setName(String name){ clientName_=name; }

        bool reportStatus(String statusName, String statusValue){
            client.publish (clientName_+"/status/"+ statusName, statusValue);
            
            Serial.print("publishing "+clientName_+"/status/"+ statusName );
            Serial.println(client.lastError());
            
            return client.lastError();
        }
        
        bool publish(const String &topic, const String &payload){ 
            return this->client.publish(topic.c_str(), payload.c_str());
        }
        
        bool loop(){
            return this->client.loop();
        }

        bool reportStatus(String statusName, bool statusValue){
            String payload= String(int(statusValue));
            return reportStatus(statusName, payload);
        }
        bool reportMetric(String metricName, String metricValue){
            client.publish (clientName_+"/metric/"+ metricName, metricValue);
        
            Serial.print("publishing "+clientName_+"/metric/"+ metricName );
            Serial.println(client.lastError());
        
            return client.lastError();
        }
        bool reportMetric(String metricName, float metricValue){
            String payload= String(metricValue);
            return reportMetric(metricName, payload);
        }
        
        bool connect(const char clientId[], bool skip = false) { 
            return this->connect(clientId, nullptr, nullptr, skip); }
        bool connect(const char clientId[], const char username[], bool skip = false) {
            return this->connect(clientId, username, nullptr, skip);
        }
        bool connect(const char clientID[], const char username[], const char password[], bool skip = false){
            return this->client.connect(clientID, username, password, skip);
        };

        bool subscribe(const String &topic) { return this->subscribe(topic.c_str()); }
        bool subscribe(const String &topic, int qos) { return this->subscribe(topic.c_str(), qos); }
        bool subscribe(const char topic[]) { return this->subscribe(topic, 0); }
        bool subscribe(const char topic[], int qos){ return this->client.subscribe(topic, qos);};

        lwmqtt_err_t lastError() { return this->client.lastError(); }
        bool connected() {return this->client.connected();}


    private:
        WiFiClient *net_;
        MQTTClient client = MQTTClient(1024);
        String clientName_;


};