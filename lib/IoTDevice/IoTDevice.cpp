#include <ESP8266WiFi.h>
#include <TempSensor.h>


class IotDevice{
  public:
    IotDevice(String deviceId){
      id_=deviceId;

    }
    String id(){return id_;}
    
    bool addSensor(TempSensor sensor){
      bool bResult=true;
      u8_t slot=firstFreeSensorSlot();
      if (slot>=0)
      {
        sensors_[slot]= &sensor;
      }
      return bResult;
    }
    
    void setName(String name){
      name_=name;
    }

    String name(){
      return name_;
    }

    bool bInitialized(){
      return bInitialized_;
    }
    void initialize(){
      bInitialized_=true;
    }

  private:
    String id_;
    TempSensor *sensors_[10];
    String name_;
    bool bInitialized_=false;
    
    u8_t firstFreeSensorSlot(){
      for (size_t i = 0; i < 10; i++)
      {
        //null=>free slot
        if(sensors_[i]) return i;
      }
      return -1;
    }
};

