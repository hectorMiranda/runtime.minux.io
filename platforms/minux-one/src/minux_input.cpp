#include "minux_input.h"

MinuxInput::MinuxInput() {
  debounce_delay = 50;
  buzzer_enabled = true;
  for(int i = 0; i < 4; i++) {
    btn_states[i] = BTN_RELEASED;
    last_press[i] = 0;
  }
}

void MinuxInput::init(uint8_t a, uint8_t up, uint8_t down, uint8_t right, uint8_t buzzer) {
  pin_a = a;
  pin_up = up;
  pin_down = down;
  pin_right = right;
  pin_buzzer = buzzer;
  
  pinMode(pin_a, INPUT_PULLUP);
  pinMode(pin_up, INPUT_PULLUP);
  pinMode(pin_down, INPUT_PULLUP);
  pinMode(pin_right, INPUT_PULLUP);
  pinMode(pin_buzzer, OUTPUT);
}

InputEvent MinuxInput::poll() {
  static unsigned long lastPoll = 0;
  
  if (millis() - lastPoll < 10) return EVENT_NONE;
  lastPoll = millis();
  
  // Check for button presses
  if (!digitalRead(pin_a) && btn_states[0] == BTN_RELEASED) {
    btn_states[0] = BTN_PRESSED;
    last_press[0] = millis();
    if (buzzer_enabled) playTone(440, 50);
    return EVENT_BTN_A;
  }
  
  if (!digitalRead(pin_up) && btn_states[1] == BTN_RELEASED) {
    btn_states[1] = BTN_PRESSED;
    last_press[1] = millis();
    if (buzzer_enabled) playTone(523, 50);
    return EVENT_BTN_UP;
  }
  
  if (!digitalRead(pin_down) && btn_states[2] == BTN_RELEASED) {
    btn_states[2] = BTN_PRESSED;
    last_press[2] = millis();
    if (buzzer_enabled) playTone(392, 50);
    return EVENT_BTN_DOWN;
  }
  
  if (!digitalRead(pin_right) && btn_states[3] == BTN_RELEASED) {
    btn_states[3] = BTN_PRESSED;
    last_press[3] = millis();
    if (buzzer_enabled) playTone(349, 50);
    return EVENT_BTN_RIGHT;
  }
  
  // Reset button states
  for(int i = 0; i < 4; i++) {
    uint8_t pins[] = {pin_a, pin_up, pin_down, pin_right};
    if (digitalRead(pins[i]) && btn_states[i] == BTN_PRESSED) {
      if (millis() - last_press[i] > debounce_delay) {
        btn_states[i] = BTN_RELEASED;
      }
    }
  }
  
  return EVENT_NONE;
}

void MinuxInput::playTone(uint16_t freq, uint16_t duration) {
  if (buzzer_enabled) {
    tone(pin_buzzer, freq, duration);
  }
}

ButtonState MinuxInput::getButtonState(uint8_t button) {
  if (button < 4) return btn_states[button];
  return BTN_RELEASED;
}

bool MinuxInput::isComboPressed() {
  int pressed = 0;
  for(int i = 0; i < 4; i++) {
    if (btn_states[i] == BTN_PRESSED) pressed++;
  }
  return pressed > 1;
}

void MinuxInput::update() {
  poll();
}
