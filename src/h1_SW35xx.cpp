#include <Arduino.h>
#include "h1_SW35xx.h"
#include <Wire.h>

namespace h1_SW35xx {

SW35xx::SW35xx(TwoWire &i2c) : _i2c(i2c) {}
SW35xx::~SW35xx() {}

void SW35xx::readStatus() {
  uint16_t tmp;
  //读取输入电压
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_TYPE);
  _i2c.write(0x01);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_H);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp = _i2c.read() << 4;
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_L);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp += _i2c.read() | 0x0f;
  vin_mV = tmp * 10;
  tmp = 0;
  //读取输出电压
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_TYPE);
  _i2c.write(0x02);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_H);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp = _i2c.read() << 4;
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_L);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp += _i2c.read() | 0x0f;
  vout_mV = tmp * 6;
  tmp = 0;
  //读取接口1输出电流
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_TYPE);
  _i2c.write(0x03);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_H);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp = _i2c.read() << 4;
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_L);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp += _i2c.read() | 0x0f;

  if (tmp > 15) //在没有输出的情况下读到的数据是15
    iout1_mA = tmp * 5 / 2;
  else
    iout1_mA = 0;
  tmp = 0;

  //读取接口2输出电流
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_TYPE);
  _i2c.write(0x04);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_H);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp = _i2c.read() << 4;
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_ADC_DATA_BUF_L);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp += _i2c.read() | 0x0f;
  if (tmp > 15)
    iout2_mA = tmp * 5 / 2;
  else
    iout2_mA = 0;
  tmp = 0;
  //读取pd版本和快充协议
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_FCX_STATUS);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  tmp = _i2c.read();
  PDVersion = ((tmp & 0x30) >> 4) + 1;
  fastChargeType = (fastChargeType_t)(tmp & 0x0f);
}
void SW35xx::readConfig() {

}

static void unlock_i2c_write(TwoWire &_i2c) {
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_ENABLE);
  _i2c.write(0x20);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_ENABLE);
  _i2c.write(0x40);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_ENABLE);
  _i2c.write(0x80);
  _i2c.endTransmission();
}

static void lock_i2c_write(TwoWire& _i2c) {
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_ENABLE);
  _i2c.write(0x00);
  _i2c.endTransmission();
}

void SW35xx::setMaxCurrent5A() {
  unlock_i2c_write(_i2c);
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF1);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF2);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF3);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF4);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF6);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF7);
  _i2c.write(0b01100100);
  _i2c.endTransmission();
  lock_i2c_write(_i2c);
}

} // namespace h1_SW35xx