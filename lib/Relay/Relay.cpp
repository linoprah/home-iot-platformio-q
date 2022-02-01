#include <ESP8266WiFi.h>
class Relay{
  public:
    Relay(int8_t pin){
      pin_=pin;
      state_=false;
      pinMode(pin_, OUTPUT);
    }
    int8_t pin(){return pin_;}
    bool state(){return state_;}
    bool switchOn(){
      state_=true;
      setPinLevel();
      return state_;
    }
    bool switchOff(){
      state_=false;
      setPinLevel();
      return state_;

    }
    bool toggle(){
      state_=(!state_);
      setPinLevel();
      return state_;
    }

  private:
    int8_t pin_;
    bool state_;
    void setPinLevel(){
        digitalWrite(pin_, state_);
    }
};
