#include "data_structs.h"
#include "unity.h"
MCB mcb_data = {
  .batteryVoltage = 12.6,
  .GPSlal = 32.43,
  .GPSlong = 123.32,
  .GPSalt = 3213,
  .GPSsat = 9,
  .GPSsec = 12,
};

void test_set_mcb_data(void) {
  char buffer[200];

  DF_set_mcb_data(&mcb_data);
  TEST_ASSERT_EQUAL(false, DF_create_mcb_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_mcb_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_mcb_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_mcb_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0;12.60;0;32.4300;123.3200;3213.00;9;12;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;", buffer);
  // TEST_ASSERT_EQUAL_STRING("", buffer);
}

PitotData pitot_data = {
  .wakenUp = 1,
  .vBat = 13.5,
  .statPress = 123.43,
  .dynamicPress = 3213.12,
  .temp = 12.7
};

void test_set_pitot_data(void) {
  char buffer[100];

  DF_set_pitot_data(&pitot_data);
  TEST_ASSERT_EQUAL(false, DF_create_pitot_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_pitot_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_pitot_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_pitot_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1;13.50;123.43;3213.12;12.70;0;0;0;0;0;", buffer);
}

MainValveData main_valve_data = {
  .wakeUp = 1,
  .valveState = 2,
  .thermocouple = {12.52, 32.65},
  .batteryVoltage = 12.43,
};

void test_set_main_valve_data(void) {
  char buffer[100];
  
  DF_set_main_valve_data(&main_valve_data);
  TEST_ASSERT_EQUAL(false, DF_create_main_valve_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_main_valve_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_main_valve_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_main_valve_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1;12.43;2;12.52;32.65;", buffer);
}

TanWaData tanwa_data = {
  .tanWaState = 3,
  .tankHeating = 1,
  .abortButton = 0,
  .igniterContinouity = {1, 1},
  .vbat = 21.37,
};

void test_set_tanwa_frame(void) {
  char buffer[100];

  DF_set_tanwa_data(&tanwa_data);
  TEST_ASSERT_EQUAL(false, DF_create_tanwa_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_tanwa_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_tanwa_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_tanwa_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("3;21.37;1;1;0;0;0;0;0;0.00;0.00;0;0;0.00;0.00;0.00;0;0;1;", buffer);
}

UpustValveData upust_data = {
  .wakeUp = 1,
  .valveState = 1,
  .thermistor = 1321,
  .batteryVoltage = 21.37,
};

void test_set_upust_valve_frame(void) {
  char buffer[100];

  DF_set_upust_valve_data(&upust_data);
  TEST_ASSERT_EQUAL(false, DF_create_upust_valve_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_upust_valve_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_upust_valve_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_upust_valve_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1;21.37;1;0.00;1321;", buffer);
}

RecoveryData recover_data = {
  .isArmed = 1,
  .firstStageContinouity = 1,
  .secondStageContinouity = 1,
  .separationSwitch1 = 0,
  .separationSwitch2 = 1,
};

void test_set_recovery_frame(void) {
  char buffer[100];

  DF_set_recovery_data(&recover_data);
  TEST_ASSERT_EQUAL(false, DF_create_recovery_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_recovery_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_recovery_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_recovery_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1;1;1;0;1;0;0;0;0;0;0;0;0;0;", buffer);
}

SlaveData blackbox_data = {
  .wakeUp = 1,
};

void test_set_blackbox_frame(void) {
  char buffer[100];
  DF_set_blackbox_data(&blackbox_data);
  TEST_ASSERT_EQUAL(false, DF_create_blackbox_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_blackbox_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_blackbox_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_blackbox_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1;", buffer);
}

PayloadData payload_data = {
  .wakenUp = 1,
  .isRecording = 1,
  .data = 0,
  .vBat = 21.37,
  .isRpiOn = 1,
};

void test_set_payload_frame(void) {
  char buffer[100];
  DF_set_payload_data(&payload_data);
  TEST_ASSERT_EQUAL(false, DF_create_payload_frame(NULL, 12));
  TEST_ASSERT_EQUAL(false, DF_create_payload_frame(buffer, 0));
  TEST_ASSERT_EQUAL(false, DF_create_payload_frame(NULL, 0));
  TEST_ASSERT_EQUAL(true, DF_create_payload_frame(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("21.37;1;1;0;1;", buffer);
}

const char SD_FRAME[] = "SD;0;12.60;0;32.4300;123.3200;3213.00;9;12;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;0.00;"
  "1;13.50;123.43;3213.12;12.70;0;0;0;0;0;1;12.43;2;12.52;32.65;"
  "3;21.37;1;1;0;0;0;0;0;0.00;0.00;0;0;0.00;0.00;0.00;0;0;1;1;21.37;1;0.00;1321;1;1;1;"
  "0;1;0;0;0;0;0;0;0;0;0;1;21.37;1;1;0;1;";

void test_create_sd_frame(void) {
  char buffer[512];
  sprintf(buffer, "SD;");
  DF_set_mcb_data(&mcb_data);
  DF_set_pitot_data(&pitot_data);
  DF_set_main_valve_data(&main_valve_data);
  DF_set_tanwa_data(&tanwa_data);
  DF_set_upust_valve_data(&upust_data);
  DF_set_recovery_data(&recover_data);
  DF_set_blackbox_data(&blackbox_data);
  DF_set_payload_data(&payload_data);

  DF_create_sd_frame(buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING(SD_FRAME, buffer);
}

// TODO: check correctnes of lora_frame
const char LORA_FRAME[] = "LR;0;12.6;32.4300;123.3200;9;12;0;0.0;13.5;0;0;12.4;21.4;"
                          "0.0;1321;3;21.4;1;1;0.00;0.00;0;0;1;1;21.4;31;6;0;0;116;0;";

void test_create_lora_frame(void) {
  char buffer[256];
  sprintf(buffer, "LR;");
  DF_set_mcb_data(&mcb_data);
  DF_set_pitot_data(&pitot_data);
  DF_set_main_valve_data(&main_valve_data);
  DF_set_tanwa_data(&tanwa_data);
  DF_set_upust_valve_data(&upust_data);
  DF_set_recovery_data(&recover_data);
  DF_set_blackbox_data(&blackbox_data);
  DF_set_payload_data(&payload_data);

  DF_create_lora_frame(buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL_STRING(LORA_FRAME, buffer);
}


void data_structs_setup(void){
    DF_init();
}

void run_data_structs_tests(void) { 
  RUN_TEST(test_set_mcb_data);
  RUN_TEST(test_set_pitot_data);
  RUN_TEST(test_set_main_valve_data);
  RUN_TEST(test_set_tanwa_frame);
  RUN_TEST(test_set_upust_valve_frame);
  RUN_TEST(test_set_recovery_frame);
  RUN_TEST(test_set_blackbox_frame);
  RUN_TEST(test_set_payload_frame);
  RUN_TEST(test_create_sd_frame);
  RUN_TEST(test_create_lora_frame);
}