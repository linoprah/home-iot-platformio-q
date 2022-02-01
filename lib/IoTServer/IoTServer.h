#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#include <ESP8266WiFi.h>
#include <MQTT.h>

class IoTServer{
    public:
        IoTServer(String server, String clientName, Client &net, MQTTClientCallbackSimple callback);
        
        void setName(String name);

        bool reportStatus(String statusName, String statusValue);
        
        bool publish(const String &topic, const String &payload);
        
        bool loop();

        bool reportStatus(String statusName, bool statusValue);
        bool reportMetric(String metricName, String metricValue);
        bool reportMetric(String metricName, float metricValue);
        
        bool connect(const char clientId[], bool skip = false);
        bool connect(const char clientId[], const char username[], bool skip = false);
        bool connect(const char clientID[], const char username[], const char password[], bool skip = false);

        bool subscribe(const String &topic);
        bool subscribe(const String &topic, int qos);
        bool subscribe(const char topic[]);
        bool subscribe(const char topic[], int qos);

        lwmqtt_err_t lastError();
        bool connected();

    private:
        WiFiClient *net_;
        MQTTClient client = MQTTClient(1024);
        String clientName_;
};

#endif
