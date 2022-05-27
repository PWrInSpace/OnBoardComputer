#include "../include/sensors/imuAPI.h"

ImuAPI::ImuAPI(TwoWire* _wire, uint8_t accgyrAddress, uint8_t magAddress, uint8_t prsAddress):
  imu(LSM6(_wire, accgyrAddress)), 
  mag(LIS3MDL(_wire, magAddress)),
  ps(LPS(_wire, prsAddress))
{
  gyroScale = GyroscpoeScale::G_245dps;
  accScale = AccelerometerScale::A_2g;
  bandwith = Bandwith::B_400Hz;
  magScale = MagnetometerScale::M_4g;
  initPressure = 1023.0;
}



bool ImuAPI::begin(){
    if (!imu.init()){
        Serial.println("Failed to detect and initialize IMU :C");
        return false;

    }else if(!ps.init()){
        Serial.println("Filed to detect and initialize PS :C");
        return false;
    
    }else if(!mag.init()){
        Serial.println("Filed to detect and initialize MAG :C");
        return false;
    
    }
    //imu.enableDefault();
    this->LSM6SetReg();
    ps.enableDefault();
    mag.enableDefault();
    return true;
}

void ImuAPI::setReg(AccelerometerScale _accScale, GyroscpoeScale _gyrScale, Bandwith _bandwith, MagnetometerScale _magScale){
  gyroScale = _gyrScale;
  accScale = _accScale;
  bandwith = _bandwith;
  magScale = _magScale;

  this->LSM6SetReg();
  ps.enableDefault();
  this->LIS3MDSetReg();
}

bool ImuAPI::setInitPressure(){
  float press = 0;
  float tempPress = 0;
  int measurement = 0;

  for(int i = 0; i<5; ++i){
    tempPress = ps.readPressureMillibars();
    measurement++;

    if(abs(1023 - tempPress) < 75){
      press += tempPress;
    }else{
      i--;
    }

    if(measurement > 10){
      initPressure = 1023;
        return false;
    }
        
    delay(10);
  }
    
  initPressure = press/5.0;

  if(ps.readPressureMillibars() - abs(initPressure) > 5){
    initPressure = 1023; //average sea level pressure
    return false;
  }    

  return true;
}

void ImuAPI::readData(){
  imu.read();
  mag.read();

  data.ax = imu.a.x * accFactor[accScale] / 1000.0;
  data.ay = imu.a.y * accFactor[accScale] / 1000.0;
  data.az = imu.a.z * accFactor[accScale] / 1000.0;

  data.gx = imu.g.x * gyroFactor[gyroScale] / 1000.0;
  data.gy = imu.g.y * gyroFactor[gyroScale] / 1000.0;
  data.gz = imu.g.y * gyroFactor[gyroScale] / 1000.0;

  data.mx = mag.m.x / magnetometerGain[magScale];
  data.my = mag.m.y / magnetometerGain[magScale];
  data.mz = mag.m.z / magnetometerGain[magScale];

  data.pressure = ps.readPressureMillibars();
  data.temperature = ps.readTemperatureC();
  data.altitude = ps.pressureToAltitudeMeters(data.pressure, initPressure);
}

float ImuAPI::getInitPressure(){
  return initPressure;
}

/*
void ImuAPI::mahonyFilterUpdate(){
  static uint32_t lastExecuteTime = {};
  float dt = {};

  Vector acc({data.ax, data.ay, data.az});
  Vector gDirection(3); //estimated direction of gravity
  Vector error(3); //imu error according to estimated and messured direction of gravity
  Vector theta(3); //rotary angle
  Quaternion dq;

  if(lastExecuteTime > 0) dt = (millis() - lastExecuteTime) / 1000.0;
  lastExecuteTime = millis();
  data.gx = 0;
  data.gy = 0;
  data.gz = 0;
  if(!(acc[0] == 0.0 && acc[1] == 0.0 && acc[2] == 0.0)){
    acc.normalize();
    Serial.print(acc[0]);
    Serial.print("\t");
    Serial.print(acc[1]);
    Serial.print("\t");
    Serial.print(acc[2]);
    Serial.println("");
    //direction of gravity
    gDirection[0] = q[1] * q[3] - q[0] * q[2];
    gDirection[1] = q[0] * q[1] + q[2] * q[3];
    gDirection[2] = q[0] * q[0] -0.5f + q[3] * q[3];
  
    //calculate error -> cross prodcut of estimated and messured direction of gravity
    error[0] = acc[1] * gDirection[2] - acc[2] * gDirection[1];
    error[1] = acc[2] * gDirection[0] - acc[0] * gDirection[2];
    error[2] = acc[0] * gDirection[1] - acc[1] * gDirection[0];
  
    Serial.print(error[0]);
    Serial.print("\t");
    Serial.print(error[1]);
    Serial.print("\t");
    Serial.print(error[2]);
    Serial.println("");
    
    if(Ki > 0){
      errorInteg[0] += Ki* error[0] * dt; 
      errorInteg[1] += Ki* error[1] * dt;
      errorInteg[2] += Ki* error[2] * dt;
      Serial.print(errorInteg[0]);
      Serial.print("\t");
      Serial.print(errorInteg[1]);
      Serial.print("\t");
      Serial.print(errorInteg[2]);
      Serial.println("");
      //integral feedback
      data.gx += errorInteg[0];
      data.gy += errorInteg[1];
      data.gz += errorInteg[2];
    }else{
      errorInteg[0] = 0;
      errorInteg[1] = 0;
      errorInteg[2] = 0;
    }

    //proportional feedback
    //data.gx += Kp * error[0];
    //data.gy += Kp * error[1];
    //data.gz += Kp * error[2];
    
    data.gx += 2.0f * error[0];
    data.gy += 2.0f * error[1];
    data.gz += 2.0f * error[2];
  
  }
  Serial.println(dt);
  //calculate rotaty angle
  //theta[0] = 0.5 * data.gx * dt;
  //theta[1] = 0.5 * data.gy * dt;
  //theta[2] = 0.5 * data.gz * dt;
  data.gx = 0.5  * dt;
  data.gy = 0.5  * dt;
  data.gz = 0.5  * dt;


  //upadte quaternion Rung Kutta Algorithm
  //dq[0] = -q[1] * theta[0] - q[2] * theta[1] - q[3] * theta[2];
  //dq[1] = q[0] * theta[0] + q[2] * theta[2] - q[3] * theta[1];
  //dq[2] = q[0] * theta[1] - q[1] * theta[2] + q[3] * theta[0];
  //dq[3] = q[0] * theta[2] + q[1] * theta[1] - q[2] * theta[0];
  
  dq[0] = -q[1] * data.gx - q[2] * data.gy - q[3] * data.gz;
  dq[1] = q[0] * data.gx + q[2] * data.gz - q[3] * data.gy;
  dq[2] = q[0] * data.gy - q[1] * data.gz + q[3] * data.gx;
  dq[3] = q[0] * data.gz + q[1] * data.gy - q[2] * data.gx;

  q[0] += dq[0];
  q[1] += dq[1];
  q[2] += dq[2];
  q[3] += dq[3];

  //normalize
  q.normalize();
}
void ImuAPI::qToPYW(){
  //TO DO
  return;
} 

Point3D ImuAPI::getDirectionVector(){
  return Point3D{q[1], q[2], q[3]};
}

void ImuAPI::setIntegralTerm(float _newKi){
  Ki = _newKi;
}
*/

ImuData ImuAPI::getData(){
  return data;
}

float ImuAPI::getAltitude(){
  readData();
  return data.altitude;
}

void ImuAPI::LSM6SetReg(){
  uint8_t reg = 0b00000000;
  
  //accelerometr
  reg |= accReg[accScale];
  reg |= bandwithReg[bandwith];
  reg |= 1 << 7; //ODR high performance 1.66kHz TO DO: odr choice
    
  imu.writeReg(imu.regAddr::CTRL1_XL, reg);
    
    //gyroscope 
  reg = 0b00000000;
  reg |= gyroReg[gyroScale];
  reg |= 1<<7; //ODR high performance 1.66kHz

  imu.writeReg(imu.regAddr::CTRL2_G, reg);
    //Comon
  reg = 0b00000100; //IF_IN = 1 automatically increment register address
  imu.writeReg(imu.regAddr::CTRL3_C, reg);
}


void ImuAPI::LIS3MDSetReg(){
  uint8_t reg = 0x00;

  reg |= magnetometerReg[magScale];
  mag.writeReg(mag.regAddr::CTRL_REG2, reg);
}