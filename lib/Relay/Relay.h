#ifndef RELAY_H
#define RELAY_H


#include <ESP8266WiFi.h>
class Relay{
  public:
    Relay(int8_t pin);
    int8_t pin();
    bool state();
    bool switchOn();
    bool switchOff();
    bool toggle();

  private:
    int8_t pin_;
    bool state_;
    void setPinLevel();
};

#endif