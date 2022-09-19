#include <Arduino.h>
#include "unity.h"
#include "tests.h"

void setUp(void) {
    options_setup();
    errors_setup();
}

void tearDown(void) {
  // clean stuff up here
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    run_options_tests();
    run_errors_tests();
    UNITY_END();
}


void loop() {}