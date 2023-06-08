#include "SimonLED.hpp"

SimonLED* SimonLED::blinkingLED = NULL;

SimonLED::SimonLED(int pinNumber) : m_pinNumber(pinNumber), m_tone(0), m_blinkDuration(500), m_blinkStartTime(0) {
  pinMode(m_pinNumber, OUTPUT);
}

SimonLED::SimonLED(int pinNumber, int buzzerPin, int tone) : m_pinNumber(pinNumber), m_buzzerPin(buzzerPin), m_tone(tone), m_blinkDuration(500), m_blinkStartTime(0) {
  pinMode(m_pinNumber, OUTPUT);
}

void SimonLED::setBuzzerPin(int buzzerPin) {
  m_buzzerPin = buzzerPin;
}

void SimonLED::setBuzzerTone(int tone) {
  m_tone = tone;
}

void SimonLED::setPinNumber(int pinNumber) {
  m_pinNumber = pinNumber;
}

int SimonLED::getPinNumber(void) const {
  return m_pinNumber;
}

void SimonLED::setState(int state) {
  digitalWrite(m_pinNumber, state);
}

void SimonLED::blinkLED(unsigned int blinkDuration) {
  if(blinkingLED != NULL) {
    blinkingLED->stopBlink();
  }
  
  blinkingLED = this;
  digitalWrite(m_pinNumber, HIGH);
  pinMode(m_buzzerPin, OUTPUT);
  tone(m_buzzerPin, m_tone, blinkDuration);
  m_blinkDuration = blinkDuration;
  m_blinkStartTime = millis();
}

void SimonLED::loop(void) {
  if(m_blinkStartTime > 0 && millis() - m_blinkStartTime >= m_blinkDuration) {
    SimonLED::stopBlink();
  }
}

void SimonLED::stopBlink(void) {
  blinkingLED = NULL;
  digitalWrite(m_pinNumber, LOW);
  noTone(m_buzzerPin);
  pinMode(m_buzzerPin, INPUT);
  m_blinkStartTime = 0;
}
