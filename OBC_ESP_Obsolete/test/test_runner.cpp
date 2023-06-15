#include <Arduino.h>
#include "unity.h"
#include "tests.h"

void setUp(void) {
    options_setup();
    errors_setup();
    data_structs_setup();
}

void tearDown(void) {
  // clean stuff up here
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    run_options_tests();
    run_errors_tests();
    run_data_structs_tests();
    UNITY_END();
}


void loop() {}