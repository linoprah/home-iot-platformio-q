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
    virtual float getT(){return nanf("");};
    virtual float getRh(){return nanf("");};
    //virtual bool initialize(){};
};

class DHTTempSensor: public TempSensor{
  public:
    virtual float getT(){
      return dht_->readTemperature();
    }
    virtual float getRh(){
      return dht_->readHumidity();
    }
    protected:
      DHT *dht_;
};

class DHT11TempSensor: public DHTTempSensor{
  public:
    DHT11TempSensor( int8_t pin){
      dht_ = new DHT(pin, DHT11);
      dht_->begin();
      Serial.println("DHT11");
    };
};

class DHT22TempSensor: public DHTTempSensor{
  public:
    DHT22TempSensor( int8_t pin){
      dht_ = new DHT(pin, DHT11);
      dht_->begin();
      Serial.println("DHT22");
    };
};

class DallasTempSensor: public TempSensor{
  public:
    DallasTempSensor( int8_t pin){

      oneWire_= new OneWire(pin);
      sensors_= new DallasTemperature(oneWire_);
      sensors_->begin();
    };
    float getT(){
      sensors_->requestTemperatures();
      return sensors_->getTempCByIndex(0);
    }
    float getRh(){
      return -1;
    }
    private:
      OneWire           *oneWire_;
      DallasTemperature *sensors_;


};
