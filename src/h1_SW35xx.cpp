#include <Arduino.h>
#include "h1_SW35xx.h"
#include <Wire.h>

#define SW35XX_IC_VERSION 0x01
#define SW35XX_FCX_STATUS 0x06
#define SW35XX_PWR_STATUS 0x07
#define SW35XX_I2C_ENABLE 0x12
#define SW35XX_I2C_CTRL 0x13
#define SW35XX_ADC_DATA_TYPE 0x3a
#define SW35XX_ADC_DATA_BUF_H 0x3b
#define SW35XX_PD_SRC_REQ 0x70
#define SW35XX_ADC_DATA_BUF_L 0x3c
#define SW35XX_PWR_CONF 0xa6
#define SW35XX_QC_CONF0 0xaa
#define SW35XX_VID_CONF0 0xaf
#define SW35XX_PD_CONF1 0xb0
#define SW35XX_PD_CONF2 0xb1
#define SW35XX_PD_CONF3 0xb2
#define SW35XX_PD_CONF4 0xb3
#define SW35XX_PD_CONF5 0xb4
#define SW35XX_PD_CONF6 0xb5
#define SW35XX_PD_CONF7 0xb6
#define SW35XX_PD_CONF8 0xb7
#define SW35XX_PD_CONF9 0xb8
#define SW35XX_QC_CONF1 0xb9
#define SW35XX_QC_CONF2 0xba
#define SW35XX_PD_CONF10 0xbe
#define SW35XX_VID_CONF1 0xbf

namespace h1_SW35xx {

SW35xx::SW35xx(TwoWire &i2c) : _i2c(i2c) {}
SW35xx::~SW35xx() {}


void SW35xx::begin(){
  //启用输入电压读取
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_CTRL);
  _i2c.write(0x02);
  _i2c.endTransmission();
}

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

void SW35xx::sendPDCmd(SW35xx::PDCmd_t cmd){
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_SRC_REQ);
  _i2c.write((const char)cmd);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_SRC_REQ);
  _i2c.write((const char)(cmd | 0x80));
  _i2c.endTransmission();
}

void SW35xx::rebroadcastPDO(){
 _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_I2C_CTRL);
  _i2c.write(0x03);
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

void SW35xx::setMaxCurrentsFixed(uint32_t ma_5v, uint32_t ma_9v, uint32_t ma_12v, uint32_t ma_15v, uint32_t ma_20v){
  if(ma_5v > 5000) ma_5v = 5000;
  if(ma_9v > 5000) ma_9v = 5000;
  if(ma_12v > 5000) ma_12v = 5000;
  if(ma_15v > 5000) ma_15v = 5000;
  if(ma_20v > 5000) ma_20v = 5000;

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF8);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  uint8_t tmp = _i2c.read();

  if(ma_9v == 0)
    tmp &= 0b11111011;
  else
    tmp |= 0b00000100;

  if (ma_12v == 0)
    tmp &= 0b11110111;
  else
    tmp |= 0b00001000;

  if (ma_15v == 0)
    tmp &= 0b11101111;
  else
    tmp |= 0b00010000;

  if (ma_20v == 0)
    tmp &= 0b11011111;
  else
    tmp |= 0b00100000;
  
  unlock_i2c_write(_i2c);

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF8);
  _i2c.write(tmp);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF1);
  _i2c.write(ma_5v/50);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF2);
  _i2c.write(ma_9v/50);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF3);
  _i2c.write(ma_12v / 50);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF4);
  _i2c.write(ma_15v/50);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF5);
  _i2c.write(ma_20v/50);
  _i2c.endTransmission();

  lock_i2c_write(_i2c);

}

void SW35xx::setMaxCurrentsPPS(uint32_t ma_pps1, uint32_t ma_pps2) {
  if (ma_pps1 > 5000)
    ma_pps1 = 5000;
  if (ma_pps2 > 5000)
    ma_pps2 = 5000;
   _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF8);
  _i2c.endTransmission();
  _i2c.requestFrom(0x3c, 1);
  uint8_t tmp = _i2c.read();

  if(ma_pps1 == 0)
    tmp &= 0b10111111;
  else
    tmp |= 0b01000000;

  if(ma_pps1 == 0)
    tmp &= 0b01111111;
  else
    tmp |= 0b10000000;
  
  
  unlock_i2c_write(_i2c);

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF8);
  _i2c.write(tmp);
  _i2c.endTransmission();

  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF6);
  _i2c.write(ma_pps1/50);
  _i2c.endTransmission();
  _i2c.beginTransmission(0x3c);
  _i2c.write(SW35XX_PD_CONF7);
  _i2c.write(ma_pps2/50);
  _i2c.endTransmission();

  lock_i2c_write(_i2c);
}

} // namespace h1_SW35xx