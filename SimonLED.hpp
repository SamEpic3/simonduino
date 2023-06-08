#ifndef SIMONLED_HPP
#define SIMONLED_HPP

#include "Arduino.h"

class SimonLED {
  public:
  SimonLED(int pinNumber);
  SimonLED(int pinNumber, int buzzerPin, int tone);
  void setBuzzerPin(int buzzerPin);
  void setBuzzerTone(int tone);
  void setPinNumber(int pinNumber);
  int getPinNumber() const;
  void setState(int state);
  void blinkLED(unsigned int blinkDuration);
  void loop();
  void stopBlink();
  static SimonLED* blinkingLED;

  private:
  int m_pinNumber;
  int m_buzzerPin;
  int m_tone;
  unsigned int m_blinkDuration;
  unsigned long m_blinkStartTime;
};

#endif
