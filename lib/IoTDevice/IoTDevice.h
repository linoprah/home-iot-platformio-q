#ifndef IOT_DEVICE_H
#define IOT_DEVICE_H




#include <ESP8266WiFi.h>
#include "TempSensor.h"


class IotDevice{
  public:
    IotDevice(String deviceId);
    String id();
    
    bool addSensor(TempSensor sensor);
    
    void setName(String name);

    String name();

    bool bInitialized();
    void initialize();

  private:
    String id_;
    TempSensor *sensors_[10];
    String name_;
    bool bInitialized_=false;
    
    u8_t firstFreeSensorSlot();
};


#endif