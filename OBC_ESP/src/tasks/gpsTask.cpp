#include "../include/tasks/tasks.h"


void gpsTask(void *arg){
  SFE_UBLOX_GNSS gps;
  
  if (gps.begin(rc.hardware.i2c2, 0x42, 10) == false) //Connect to the u-blox module using Wire port
  {
    rc.sendLog("GPS INIT ERROR");
    rc.errors.setSensorError(GPS_INIT_ERROR);
  }

  gps.setI2COutput(COM_TYPE_UBX);


  while(1){
     rc.dataFrame.mcb.GPSlal = gps.getLatitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSlong = gps.getLongitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSalt = gps.getAltitude(10) / 10.0E2;
        
      rc.dataFrame.mcb.GPSsat = gps.getSIV(10);
      rc.dataFrame.mcb.GPSsec = gps.getTimeValid(10);
      //portEXIT_CRITICAL(&rc.hardware.stateLock);
      Serial.println("====GPS DATA====");
      Serial.print("Lat: ");
      Serial.println(rc.dataFrame.mcb.GPSlal);
      
      Serial.print("Long: ");
      Serial.println(rc.dataFrame.mcb.GPSlong);
      
      Serial.print("Alt: ");
      Serial.println(rc.dataFrame.mcb.GPSalt);
      
      Serial.print("Sat: ");
      Serial.println(rc.dataFrame.mcb.GPSsat);
      
      Serial.print("Valid: ");
      Serial.println(rc.dataFrame.mcb.GPSsec);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}