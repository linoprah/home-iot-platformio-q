#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H


#include <ESP8266WiFi.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>


class DHTTempSensor;
class DHT11TempSensor;
class DHT22TempSensor;
class DallasTempSensor;


class TempSensor{
  public:
    virtual float getT();
    virtual float getRh();
    //virtual bool initialize(){};
};

class DHTTempSensor: public TempSensor{
  public:
    virtual float getT();
    virtual float getRh();
    protected:
      DHT *dht_;
};

class DHT11TempSensor: public DHTTempSensor{
  public:
    DHT11TempSensor( int8_t pin);
};

class DHT22TempSensor: public DHTTempSensor{
  public:
    DHT22TempSensor( int8_t pin);
};

class DallasTempSensor: public TempSensor{
  public:
    DallasTempSensor( int8_t pin);
    float getT();
    float getRh();
    private:
      OneWire           *oneWire_;
      DallasTemperature *sensors_;
};

#endif