/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "command_responder.h"

#include "Arduino.h"

// Use the RGB LED
#include "PinNames.h"
const PinName LED_GREEN = PinName::p16;
const PinName LED_RED = PinName::p24;
const PinName LED_BLUE = PinName::p6;
const unsigned int DELAY = 1000/6;

// Toggles the LED every inference, and keeps it on for ~2 seconds if a "yes"
// was heard
void RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {
  static bool is_initialized = false;
  if (!is_initialized) {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);

    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    is_initialized = true;
  }
  static int32_t last_yes_time = 0;
  static int32_t last_no_time = 0;
  static int32_t last_uk_time = 0;
  static int count = 0;

  if (is_new_command) {
    error_reporter->Report("Heard %s (%d) @%dms", found_command, score,
                           current_time);
    // If we heard a "yes" or "no", switch on an LED and store the time.
    if (found_command[0] == 'y') {
      last_yes_time = current_time;
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LED_GREEN, LOW);
    }
    if (found_command[0] == 'n') {
      last_no_time = current_time;
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LED_RED, LOW);
    }
    if (found_command[0] == 'u') {
      last_uk_time = current_time;
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
  }

  // If last_yes_time is non-zero but was >3 seconds ago, zero it
  // and switch off the LED.
  if (last_yes_time != 0) {
    if (last_yes_time < (current_time - 3000)) {
      last_yes_time = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(LED_GREEN, HIGH);
    }
    // If it is non-zero but <3 seconds ago, do nothing.
    return;
  }

  // If last_no_time is non-zero but was >3 seconds ago, zero it
  // and switch off the LED.
  if (last_no_time != 0) {
    if (last_no_time < (current_time - 3000)) {
      last_no_time = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(LED_RED, HIGH);
    }
    // If it is non-zero but <3 seconds ago, do nothing.
    return;
  }

  // If last_no_time is non-zero but was >3 seconds ago, zero it
  // and switch off the LED.
  if (last_uk_time != 0) {
    if (last_uk_time < (current_time - 3000)) {
      last_uk_time = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, HIGH);
    }
    // If it is non-zero but <3 seconds ago, do nothing.
    return;
  }

  // Otherwise, toggle the LED every time an inference is performed.
  ++count;
  if (count & 1) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
