#ifndef MINUX_INPUT_H
#define MINUX_INPUT_H

#include <Arduino.h>

// Button states
enum ButtonState {
  BTN_RELEASED,
  BTN_PRESSED,
  BTN_HELD
};

// Input events
enum InputEvent {
  EVENT_NONE,
  EVENT_BTN_A,
  EVENT_BTN_UP,
  EVENT_BTN_DOWN,
  EVENT_BTN_RIGHT,
  EVENT_BTN_COMBO
};

class MinuxInput {
private:
  uint8_t pin_a, pin_up, pin_down, pin_right, pin_buzzer;
  ButtonState btn_states[4];
  unsigned long last_press[4];
  unsigned long debounce_delay;
  bool buzzer_enabled;
  
public:
  MinuxInput();
  void init(uint8_t a, uint8_t up, uint8_t down, uint8_t right, uint8_t buzzer);
  InputEvent poll();
  void playTone(uint16_t freq, uint16_t duration);
  void enableBuzzer(bool enable) { buzzer_enabled = enable; }
  ButtonState getButtonState(uint8_t button);
  void handleButtonPress(uint8_t button);
  bool isComboPressed(); // Check for multiple buttons
  void update();
};

extern MinuxInput input;

#endif
