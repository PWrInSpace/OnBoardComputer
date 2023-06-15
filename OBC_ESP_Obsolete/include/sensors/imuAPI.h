#ifndef IMUAPI_HH
#define IMUAPI_HH

#include "LIS3MDL.h"
#include "LPS.h"
#include "LSM6.h"
#include <Wire.h>


const uint8_t accReg[4] = {(0<<2), (1<<3), (1<<3) | (1<<2), (1<<2)};
const float accFactor[4] = {0.061, 0.122, 0.244, 0.488};
const uint8_t gyroReg[4] = {(0<<3), (1<<3), (1<<4), (1<<4) | (1<<3)};
const float gyroFactor[4] = {8.75, 17.50, 35.0, 70.0};
const uint8_t bandwithReg[4] = {(1<<1) | (1<<0), (1<<1), (1<<0), (0<<0)};
const float
 magnetometerGain[4] = {6842, 3421, 2281, 1711};
const uint8_t magnetometerReg[4] = {(0 << 6) | (0 << 5), (0 << 6) | (1 << 5), (1 << 6) | (1 << 5), (1 << 6) | (1 << 5)};

enum AccelerometerScale{
    A_2g = 0,
    A_4g,
    A_8g,
    A_16g,
};

enum GyroscpoeScale{
    G_245dps = 0,
    G_500dps,
    G_1000dps,
    G_2000dps,
};

enum MagnetometerScale{
  M_4g = 0,
  M_8g,
  M_12g,
  M_16g
};

enum Bandwith{
    B_50Hz = 0,
    B_100Hz,
    B_200Hz,
    B_400Hz,
};

struct ImuData{
  float ax;
  float ay;
  float az;
  
  float gx;
  float gy;
  float gz;
  
  float mx;
  float my;
  float mz;

  float temperature;
  float pressure;
  float altitude;
};


class ImuAPI{
  LSM6 imu;
  LIS3MDL mag;
  LPS ps;
  ImuData data;

  float initPressure;
  Bandwith bandwith;
  AccelerometerScale accScale;
  GyroscpoeScale gyroScale;
  MagnetometerScale magScale;

  //proccesing
  /*
  Quaternion q;
  float Ki;
  float Kp;
  Vector errorInteg;
  */
  public:
  ImuAPI(
    TwoWire* _wire, 
    uint8_t accgyrAdress = 0x6B, 
    uint8_t magAdress = 0x1E, 
    uint8_t prsAdress = 0x5D);
  bool begin();
  void setReg(
    AccelerometerScale _accScale,
    GyroscpoeScale _gyrScale, 
    Bandwith _bandwith, 
    MagnetometerScale magScale);
  bool setInitPressure();
  void readData();
  void setIntegralTerm(float _newKi);
  ImuData getData();
  float getAltitude();
  float getInitPressure();
    
  //void mahonyFilterUpdate();
  //void qToPYW();
  //Point3D getDirectionVector();

  private:
  void LSM6SetReg();
  void LIS3MDSetReg();
};
#endif