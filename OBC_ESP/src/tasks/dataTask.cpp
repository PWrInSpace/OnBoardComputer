#include "../include/tasks/tasks.h"
#include "Adafruit_MCP9808.h"
#include "esp_log.h"
#include "../include/timers/expire_timer.h"
#define TAG "DT"

static struct {
  MCB mcb_data;
  RecoveryData recovery;
  char sd_buffer[SD_FRAME_ARRAY_SIZE];
  char lora_buffer[LORA_FRAME_ARRAY_SIZE];

  expire_timer_t data_update_timer;
  expire_timer_t lora_timer;
  expire_timer_t flash_timer;
  expire_timer_t sd_timer;

  float apogee;
  float launchpad_altitude;
  float last_max_altitude;
  TickType_t apogeeConfirmationTimer;
}glob;

static void init_globals(void) {
  memset(&glob.mcb_data, 0, sizeof(glob.mcb_data));
  memset(glob.sd_buffer, 0 ,sizeof(glob.sd_buffer));
  memset(glob.lora_buffer, 0, sizeof(glob.lora_buffer));

  ET_init(&glob.data_update_timer);
  ET_init(&glob.lora_timer);
  ET_init(&glob.flash_timer);
  ET_init(&glob.sd_timer);
  glob.apogee = 0;
  glob.launchpad_altitude = 0;
  glob.last_max_altitude = 0;
  glob.apogeeConfirmationTimer = 0;

  ET_start(&glob.data_update_timer, 0);
  ET_start(&glob.lora_timer, 0);
  ET_start(&glob.flash_timer, 0);
  ET_start(&glob.sd_timer, 0);
}

static void block_task_and_print(char* text) {
  while(1) {
    Serial.print("Task blocked: ");
    Serial.println(__FILENAME__);
    Serial.println(text);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static void get_current_state(void) {
  glob.mcb_data.state = SM_getCurrentState();
}

static void read_battery_voltage(void) {
  glob.mcb_data.batteryVoltage = 2.93/3635.0 * analogRead(BATTERY_PIN) * 43.0/10.0 + 0.5;
}

static bool gps_init(SFE_UBLOX_GNSS & gnss) {
  if (gnss.begin(rc.hardware.i2c2, 0x42, 1100) == false) {
    rc.sendLog("GPS INIT ERROR");
    ERR_set_sensors_error(GPS_INIT_ERROR);
    return false;
  }

  gnss.setI2COutput(COM_TYPE_UBX);

  return true;
}

static void gps_read_data(SFE_UBLOX_GNSS &gnss) {
  glob.mcb_data.gps.latitude = gnss.getLatitude(10) / 10.0E6;
  glob.mcb_data.gps.longitude = gnss.getLongitude(10) / 10.0E6;
  glob.mcb_data.gps.gps_altitude = gnss.getAltitude(10) / 10.0E2;
  glob.mcb_data.gps.satellites = gnss.getSIV(10);
  glob.mcb_data.gps.is_time_valid = gnss.getTimeValid(10);
}

static bool imu_init(ImuAPI &imu) {
  if(!imu.begin() ){
    rc.sendLog("IMU INIT ERROR");
    ERR_set_sensors_error(IMU_INIT_ERROR);
    return false;
  }

  imu.setReg(A_16g, G_2000dps, B_200Hz, M_4g);
  vTaskDelay(250 / portTICK_PERIOD_MS);
  imu.setInitPressure();
  glob.launchpad_altitude = imu.getAltitude();
  return true;
}

static void imu_read_data(ImuAPI &imu) {
  imu.readData();
  ImuData imu_data = imu.getData();
  glob.mcb_data.imu.ax = imu_data.ax;
  glob.mcb_data.imu.ay = imu_data.ay;
  glob.mcb_data.imu.az = imu_data.az;
  glob.mcb_data.imu.gx = imu_data.gx;
  glob.mcb_data.imu.gy = imu_data.gy;
  glob.mcb_data.imu.gz = imu_data.gz;
  glob.mcb_data.imu.mx = imu_data.mx;
  glob.mcb_data.imu.my = imu_data.my;
  glob.mcb_data.imu.mz = imu_data.mz;
  glob.mcb_data.imu.temperature = imu_data.temperature;
  glob.mcb_data.imu.pressure = imu_data.pressure;
  glob.mcb_data.imu.altitude = imu_data.altitude;
}

static bool pressure_sensor_init(LPS25HB &sensor) {
  sensor.begin(rc.hardware.i2c2, PRESSURE_SENSOR_ADRESS);

  if (sensor.isConnected() == false){
    rc.sendLog("PRESSURE SENSOR ERROR");
    ERR_set_sensors_error(PRESSURE_SENSOR_INIT_ERROR);
    return false;
  }

  return true;
}

static void pressure_sensor_read(LPS25HB &sensor) {
  glob.mcb_data.pressure = sensor.getPressure_hPa();
  glob.mcb_data.temp_lp25 = sensor.getTemperature_degC();
  Serial.print("LPS25HB pressure: ");
  Serial.println(glob.mcb_data.pressure);
  Serial.print("LPS25HB temperature: ");
  Serial.println(glob.mcb_data.temp_lp25);
}


static bool temperature_sensor_init(Adafruit_MCP9808 &sensor) {
  if (sensor.begin(MCP_ADDRESS, &rc.hardware.i2c2) == false) {
    rc.sendLog("TEMP SENSOR INIT ERROR");
    ERR_set_sensors_error(TEMP_SENSOR_INIT_ERROR);
    return false;
  }

  sensor.setResolution(1);
  sensor.wake();
  return true;
}
static void temperature_sensor_read(Adafruit_MCP9808 &sensor) {
  glob.mcb_data.temp_mcp = sensor.readTempC();
}

static void read_recovery_data(void) {
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.getRecoveryData(glob.recovery.raw);
  xSemaphoreGive(rc.hardware.i2c1Mutex);
}

static void check_first_stage_recovery_deploy(void) {
  if(glob.mcb_data.state == FLIGHT && glob.recovery.data.firstStageDone == true){
    SM_changeStateRequest(FIRST_STAGE_RECOVERY);
    rc.sendLog("First stage recovery");
  }
}

static void check_second_stage_recovery_deploy(void) {
  if (glob.mcb_data.state == FIRST_STAGE_RECOVERY && glob.recovery.data.secondStageDone == true) {
    SM_changeStateRequest(SECOND_STAGE_RECOVERY);
    rc.sendLog("Second stage recovery");
  }
}

static void apogee_detection(void) {
  if (glob.apogee != 0) {
    return;
  }

  if (glob.mcb_data.state == States::FLIGHT){
    if(glob.last_max_altitude < glob.mcb_data.altitude){
      glob.last_max_altitude = glob.mcb_data.altitude;
    } else {
      glob.apogeeConfirmationTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
    }

    if((xTaskGetTickCount() * portTICK_PERIOD_MS - glob.apogeeConfirmationTimer) > 500){
      glob.apogee = glob.last_max_altitude;
      sprintf(glob.sd_buffer, "Apoge detected! Time %d, Altitude %f", rc.missionTimer.getTime(), rc.dataFrame.mcb.apogee);
      rc.sendLog(glob.sd_buffer);
    }
  }
}

static void landing_detection(void) {
  if (glob.mcb_data.state != States::SECOND_STAGE_RECOVERY) {
    return;
  }

  if (glob.mcb_data.altitude < (glob.launchpad_altitude + 100)) {
        SM_changeStateRequest(States::ON_GROUND);
  }
}

static void fill_lora_data_buffer(void) {
  char temp[200] = {0};
  memset(glob.lora_buffer, 0, sizeof(glob.lora_buffer));
  memcpy(glob.lora_buffer, LORA_TX_DATA_PREFIX, sizeof(LORA_TX_DATA_PREFIX));

  DF_create_lora_frame(temp, sizeof(temp));
  strcat(glob.lora_buffer, temp);
  memset(temp, 0, sizeof(temp));

  ERR_create_lora_frame(temp, sizeof(temp));
  strcat(glob.lora_buffer, temp);
  memset(temp, 0, sizeof(temp));

  // strcat(glob.lora_buffer, "\n");
}

static void clear_lora_buffer(void) {
  memset(&glob.lora_buffer, 0, sizeof(glob.lora_buffer));
}

static void send_data_via_lora(void) {
  fill_lora_data_buffer();
  Serial.println(glob.lora_buffer);
  if(xQueueSend(rc.hardware.loraTxQueue, (void*)&glob.lora_buffer, 0) != pdTRUE){
    ERR_set_rtos_error(RTOS_LORA_QUEUE_ADD_ERROR);
    rc.sendLog("LoRa quque is full");
    Serial.println("Reseting device due to timer malfunction");
    ESP.restart();
  }

  ERR_reset(ERROR_RESET_LORA);
  clear_lora_buffer();
}

static void send_data_to_black_box(void) {
  if(glob.mcb_data.state < COUNTDOWN || glob.mcb_data.state > SECOND_STAGE_RECOVERY) {
    return;
  }

  if(esp_now_send(adressBlackBox, (uint8_t*) &rc.dataFrame, sizeof(DataFrame)) != ESP_OK){
    ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    rc.sendLog("Black box send error");
  }
}

static void write_data_to_flash(void) {
  if(glob.mcb_data.state < COUNTDOWN || glob.mcb_data.state > SECOND_STAGE_RECOVERY) {
    return;
  }


  if (OPT_get_flash_write() == false) {
    return;
  }

  if(xQueueSend(rc.hardware.flashQueue, (void*)&rc.dataFrame, 0) != pdTRUE){
    ERR_set_rtos_error(RTOS_FLASH_QUEUE_ADD_ERROR);
    rc.sendLog("Flash queue is full");
  }
}

static void write_data_to_sd(void) {
  pysdmain_DataFrame sd_data;
  DF_fill_pysd_struct(&sd_data);
  OPT_fill_pysd_struct(&sd_data);
  ERR_fill_pysd_struct(&sd_data);

  if (pysd_create_sd_frame(glob.sd_buffer, sizeof(glob.sd_buffer), sd_data, false) == 0) {
    Serial.println("SD frame buffor is too smool");
    return;
  }

  if (xQueueSend(rc.hardware.sdQueue, (void*)glob.sd_buffer, 0) != pdTRUE) { //data to SD
    ERR_set_rtos_error(RTOS_SD_QUEUE_ADD_ERROR);
    return;
  }

  ERR_reset(ERROR_RESET_SD); //reset errors after save
}


static void update_current_state(uint8_t state) {
  glob.mcb_data.state = state;
}

static void update_mcb_uptime(uint32_t uptime) {
  glob.mcb_data.uptime = uptime;
}

static void update_mcb_mission_timer(int32_t timer) {
  glob.mcb_data.mission_timer = timer;
}

static void update_disconnect_remaining_time(uint32_t time) {
  glob.mcb_data.disconnect_remaining_time = time;
}

void dataTask(void *arg){
  SFE_UBLOX_GNSS gps;
  ImuAPI imu(&rc.hardware.i2c2);
  LPS25HB pressureSensor;
  Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

  init_globals();

  if (gps_init(gps) == false) {
    block_task_and_print("GPS ERROR");
  }

  // if (imu_init(imu) == false) {
  //   ESP_LOGW(TAG, "IMU init fail");
  // }

  // if (pressure_sensor_init(pressureSensor) == false) {
  //   Serial.println("Pressure sensor init fail !!!!!!");
  // }

  if (temperature_sensor_init(tempsensor) == false) {
    ESP_LOGW(TAG, "Tempreature sensor init fail");
  }
  uint32_t time = xTaskGetTickCount();
  while(1) {
    if (ET_is_expired(&glob.data_update_timer)) {
      ET_start(&glob.data_update_timer, OPT_get_data_current_period());

      get_current_state();
      read_battery_voltage();
      gps_read_data(gps);
      // imu_read_data(imu);
      // pressure_sensor_read(pressureSensor);
      temperature_sensor_read(tempsensor);
      // read_recovery_data();
      update_current_state(SM_getCurrentState());
      update_mcb_uptime(millis());
      update_mcb_mission_timer(rc.missionTimer.getTime());
      update_disconnect_remaining_time(rc.getDisconnectRemainingTime());

      //calculations
      check_first_stage_recovery_deploy();
      check_second_stage_recovery_deploy();
      apogee_detection();

      DF_set_mcb_data(&glob.mcb_data);
      DF_set_recovery_data((RecoveryData*) &glob.recovery.data);
    }

    //LORA
    if(ET_is_expired(&glob.lora_timer) || ulTaskNotifyTake(pdTRUE, 0)){
      ET_start(&glob.lora_timer, OPT_get_lora_current_period());
      Serial.print("Lora send: ");
      Serial.print(OPT_get_lora_current_period());
      Serial.print("\t");
      Serial.println(pdTICKS_TO_MS(xTaskGetTickCount()));
      send_data_via_lora();
    }

    if(ET_is_expired(&glob.flash_timer)){
      ET_start(&glob.flash_timer, OPT_get_flash_write_current_period());
      write_data_to_flash();
      // send_data_to_black_box();
    }

    if(ET_is_expired(&glob.sd_timer)){
      ET_start(&glob.sd_timer, OPT_get_sd_write_current_period());
      Serial.print("SD write: ");
      Serial.print(OPT_get_sd_write_current_period());
      Serial.print("\t");
      Serial.println(pdTICKS_TO_MS(xTaskGetTickCount()));
      write_data_to_sd();
    }


    if (xTaskGetTickCount() - time > 1000) {
      time = xTaskGetTickCount();
      Serial.println("Data task tick");
    }

    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}