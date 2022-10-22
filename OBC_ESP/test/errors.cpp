#include <Arduino.h>
#include "unity.h"
#include "errors.h"
#include "config.h"
#include "tests.h"

static void test_error_init(void) {
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(SD_NO_ERROR, err.sd);
    TEST_ASSERT_EQUAL(FLASH_NO_ERROR, err.flash);
    TEST_ASSERT_EQUAL(RTOS_NO_ERROR, err.rtos);
    TEST_ASSERT_EQUAL(ESPNOW_NO_ERROR, err.espnow);
    TEST_ASSERT_EQUAL(WATCHDOG_NO_ERROR, err.watchdog);
    TEST_ASSERT_EQUAL(SENSORS_NO_ERROR, err.sensors);
    TEST_ASSERT_EQUAL(NO_EXCEPTION, err.exceptions);
    TEST_ASSERT_EQUAL(RECOVERY_NO_ERROR, err.recovery);
}

static void test_set_sd_error(void) {
    ERR_set_sd_error(SD_WRITE_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(SD_WRITE_ERROR, err.sd);
}

static void test_set_flash_error(void) {
    ERR_set_flash_error(FLASH_WRITE_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(FLASH_WRITE_ERROR, err.flash);
}

static void test_set_rtos_error(void) {
    ERR_set_rtos_error(RTOS_FLASH_QUEUE_ADD_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(RTOS_FLASH_QUEUE_ADD_ERROR, err.rtos);
}

static void test_set_esp_now_error(void) {
    ERR_set_esp_now_error(ESPNOW_INIT_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(ESPNOW_INIT_ERROR, err.espnow);
}

static void test_set_watchdog_error(void) {
    ERR_set_watchdog_error(WATCHDOG_FLASH_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(WATCHDOG_FLASH_ERROR, err.watchdog);
}

static void test_set_sensors_error(void) {
    ERR_set_sensors_error(IMU_INIT_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(IMU_INIT_ERROR, err.sensors);
}

static void test_set_last_exception(void) {
    ERR_set_last_exception(INVALID_OPTION_NUMBER);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(INVALID_OPTION_NUMBER, err.exceptions);
}

static void test_set_recovery_error(void) {
    ERR_set_recovery_error(RECOVERY_I2C_INIT_ERROR);
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(RECOVERY_I2C_INIT_ERROR, err.recovery);
}

static void test_reset(void) {
    ERR_set_recovery_error(RECOVERY_I2C_INIT_ERROR);
    ERR_set_esp_now_error(ESPNOW_ADD_PEER_ERROR);
    ERR_set_last_exception(DEVICE_NOT_ARMED_EXCEPTION);
    ERR_set_sd_error(SD_WRITE_ERROR);
    ERR_reset(ERROR_RESET_LORA); 
    Errors err;
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(RECOVERY_I2C_INIT_ERROR, err.recovery);
    TEST_ASSERT_EQUAL(ESPNOW_ADD_PEER_ERROR, err.espnow);
    TEST_ASSERT_EQUAL(DEVICE_NOT_ARMED_EXCEPTION, err.exceptions);
    TEST_ASSERT_EQUAL(SD_WRITE_ERROR, err.sd);
    ERR_reset(ERROR_RESET_SD);
    err = ERR_get_struct();
    TEST_ASSERT_EQUAL(RECOVERY_I2C_INIT_ERROR, err.recovery);
    TEST_ASSERT_EQUAL(ESPNOW_ADD_PEER_ERROR, err.espnow);
    TEST_ASSERT_EQUAL(DEVICE_NOT_ARMED_EXCEPTION, err.exceptions);
    TEST_ASSERT_EQUAL(SD_NO_ERROR, err.sd);
}

static void test_lora_frame(void) {
    char buffer[50] = {0};
    TEST_ASSERT_EQUAL(false, ERR_create_lora_frame(buffer, 0));
    TEST_ASSERT_EQUAL(false, ERR_create_lora_frame(NULL, 12));
    TEST_ASSERT_EQUAL(false, ERR_create_lora_frame(NULL, 0));

    ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    ERR_set_rtos_error(RTOS_LORA_QUEUE_ADD_ERROR);
    ERR_set_sd_error(SD_WRITE_ERROR);
    TEST_ASSERT_EQUAL(true, ERR_create_lora_frame(buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL_STRING("64;43;2;", buffer);
}

static void test_sd_frame(void) {
    char buffer[50] = {0};
    TEST_ASSERT_EQUAL(false, ERR_create_sd_frame(buffer, 0));
    TEST_ASSERT_EQUAL(false, ERR_create_sd_frame(NULL, 12));
    TEST_ASSERT_EQUAL(false, ERR_create_sd_frame(NULL, 0));
    
    ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    ERR_set_rtos_error(RTOS_LORA_QUEUE_ADD_ERROR);
    ERR_set_sd_error(SD_WRITE_ERROR);
    TEST_ASSERT_EQUAL(true, ERR_create_sd_frame(buffer, sizeof(buffer)));
    TEST_ASSERT_EQUAL_STRING("2;0;2;3;0;0;5;0;", buffer);
}

void errors_setup(void){
    ERR_init();
}

void run_errors_tests(void) { 
  RUN_TEST(test_error_init);
  RUN_TEST(test_set_sd_error);
  RUN_TEST(test_set_flash_error);
  RUN_TEST(test_set_rtos_error);
  RUN_TEST(test_set_esp_now_error);
  RUN_TEST(test_set_watchdog_error);
  RUN_TEST(test_set_sensors_error);
  RUN_TEST(test_set_last_exception);
  RUN_TEST(test_set_recovery_error);
  RUN_TEST(test_reset);
  RUN_TEST(test_sd_frame);
  RUN_TEST(test_lora_frame);
}