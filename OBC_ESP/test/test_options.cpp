#include "unity.h"
#include <Arduino.h>
#include "options.h"
#include "config.h"


// void setUp(void) {
//   // set stuff up here
// }

// void tearDown(void) {
//   // clean stuff up here
// }

void test_lora_freq(void) {
  uint32_t freq;
  freq = OPT_get_lora_freq();
  TEST_ASSERT_EQUAL(LoRa_FREQUENCY_KHZ, freq);
  TEST_ASSERT_EQUAL(false, OPT_set_lora_freq(10000));
  TEST_ASSERT_EQUAL(false, OPT_set_lora_freq(5230000));
  TEST_ASSERT_EQUAL(true, OPT_set_lora_freq(868100));
  freq = OPT_get_lora_freq();
  TEST_ASSERT_EQUAL(868100, freq);
}

void test_countdown_begin_time(void) {
  TEST_ASSERT_EQUAL(COUNTDOWN_TIME, OPT_get_countdown_begin_time());
  TEST_ASSERT_EQUAL(false, OPT_set_countdown_begin_time(0));
  TEST_ASSERT_EQUAL(false, OPT_set_countdown_begin_time(5000));
  TEST_ASSERT_EQUAL(false, OPT_set_countdown_begin_time(-9000));
  TEST_ASSERT_EQUAL(true, OPT_set_countdown_begin_time(20000));
  TEST_ASSERT_EQUAL(-20000, OPT_get_countdown_begin_time());
  TEST_ASSERT_EQUAL(true, OPT_set_countdown_begin_time(-30000));
  TEST_ASSERT_EQUAL(-30000, OPT_get_countdown_begin_time());
}

void test_ignition_time(void) {
  TEST_ASSERT_EQUAL(IGNITION_TIME, OPT_get_ignition_time());
  TEST_ASSERT_EQUAL(false, OPT_set_ignition_time(OPT_get_countdown_begin_time()));
  TEST_ASSERT_EQUAL(false, OPT_set_ignition_time(-OPT_get_countdown_begin_time()));
  TEST_ASSERT_EQUAL(false, OPT_set_ignition_time(OPT_get_countdown_begin_time() - 5000));
  TEST_ASSERT_EQUAL(false, OPT_set_ignition_time(0));
  TEST_ASSERT_EQUAL(true, OPT_set_ignition_time(5000));
  TEST_ASSERT_EQUAL(-5000, OPT_get_ignition_time());
  TEST_ASSERT_EQUAL(true, OPT_set_ignition_time(-5000));
  TEST_ASSERT_EQUAL(-5000, OPT_get_ignition_time());
}

void test_tank_min_pressure(void) {
  TEST_ASSERT_EQUAL(TANK_MIN_PRESSURE, OPT_get_tank_min_pressure());
  TEST_ASSERT_EQUAL(true, OPT_set_tank_min_pressure(25));
  TEST_ASSERT_EQUAL(25, OPT_get_tank_min_pressure());
}

void test_flash_write(void) {
  TEST_ASSERT_EQUAL(FLASH_WRITE, OPT_get_flash_write());
  TEST_ASSERT_EQUAL(true, OPT_set_flash_write(true));
  TEST_ASSERT_EQUAL(true, OPT_get_flash_write());
  TEST_ASSERT_EQUAL(true, OPT_set_flash_write(false));
  TEST_ASSERT_EQUAL(false, OPT_get_flash_write());
}

void test_force_launch(void) {
  TEST_ASSERT_EQUAL(FORCE_LAUNCH, OPT_get_force_launch());
  TEST_ASSERT_EQUAL(true, OPT_set_force_launch(true));
  TEST_ASSERT_EQUAL(true, OPT_get_force_launch());
  TEST_ASSERT_EQUAL(true, OPT_set_force_launch(false));
  TEST_ASSERT_EQUAL(false, OPT_get_force_launch());
}

void test_data_period(void) {
  TEST_ASSERT_EQUAL(DATA_PERIOD, OPT_get_data_current_period());
  TEST_ASSERT_EQUAL(false, OPT_set_data_current_period(0));
  TEST_ASSERT_EQUAL(true, OPT_set_data_current_period(1000));
  TEST_ASSERT_EQUAL(1000, OPT_get_data_current_period());
}

void test_lora_period(void) {
  TEST_ASSERT_EQUAL(IDLE_PERIOD, OPT_get_lora_current_period());
  TEST_ASSERT_EQUAL(false, OPT_set_lora_current_period(0));
  TEST_ASSERT_EQUAL(true, OPT_set_lora_current_period(1000));
  TEST_ASSERT_EQUAL(1000, OPT_get_lora_current_period());
}

void test_flash_write_period(void) {
  TEST_ASSERT_EQUAL(FLASH_LONG_PERIOD, OPT_get_flash_write_current_period());
  TEST_ASSERT_EQUAL(false, OPT_set_flash_write_current_period(0));
  TEST_ASSERT_EQUAL(true, OPT_set_flash_write_current_period(1000));
  TEST_ASSERT_EQUAL(1000, OPT_get_flash_write_current_period());
}

void test_sd_write_period(void) {
  TEST_ASSERT_EQUAL(IDLE_PERIOD, OPT_get_sd_write_current_period());
  TEST_ASSERT_EQUAL(false, OPT_set_sd_write_current_period(0));
  TEST_ASSERT_EQUAL(true, OPT_set_sd_write_current_period(1000));
  TEST_ASSERT_EQUAL(1000, OPT_get_sd_write_current_period());
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lora_freq);
  RUN_TEST(test_countdown_begin_time);
  RUN_TEST(test_ignition_time);
  RUN_TEST(test_tank_min_pressure);
  RUN_TEST(test_flash_write);
  RUN_TEST(test_force_launch);
  RUN_TEST(test_data_period);
  RUN_TEST(test_lora_period);
  RUN_TEST(test_flash_write_period);
  RUN_TEST(test_sd_write_period);
  return UNITY_END();
}

void setup() {
  delay(2000);

  runUnityTests();
}
void loop() {}