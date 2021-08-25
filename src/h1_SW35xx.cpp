#include <Arduino.h>
#include "h1_SW35xx.h"
#include <Wire.h>

#define SW35XX_ADDRESS 0x3c
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

int SW35xx::i2cReadReg8(const uint8_t reg) {
  _i2c.beginTransmission(SW35XX_ADDRESS);
  _i2c.write(reg);
  _i2c.endTransmission();

  _i2c.requestFrom(SW35XX_ADDRESS, 1);
  return _i2c.read();
}

int SW35xx::i2cWriteReg8(const uint8_t reg, const uint8_t data) {
  _i2c.beginTransmission(SW35XX_ADDRESS);
  _i2c.write(reg);
  _i2c.write(data);
  return _i2c.endTransmission();
}

void SW35xx::begin(){
  //启用输入电压读取
  i2cWriteReg8(SW35XX_I2C_CTRL, 0x02);
}

void SW35xx::readStatus() {
  uint16_t tmp;
  //读取输入电压
  i2cWriteReg8(SW35XX_ADC_DATA_TYPE, 0x01);

  tmp = i2cReadReg8(SW35XX_ADC_DATA_BUF_H) << 4;
  tmp += i2cReadReg8(SW35XX_ADC_DATA_BUF_L) | 0x0f;
  vin_mV = tmp * 10;
  tmp = 0;
  //读取输出电压
  i2cWriteReg8(SW35XX_ADC_DATA_TYPE, 0x02);

  tmp = i2cReadReg8(SW35XX_ADC_DATA_BUF_H) << 4;
  tmp += i2cReadReg8(SW35XX_ADC_DATA_BUF_L) | 0x0f;
  vout_mV = tmp * 6;
  tmp = 0;
  //读取接口1输出电流
  i2cWriteReg8(SW35XX_ADC_DATA_TYPE, 0x03);

  tmp = i2cReadReg8(SW35XX_ADC_DATA_BUF_H) << 4;
  tmp += i2cReadReg8(SW35XX_ADC_DATA_BUF_L) | 0x0f;

  if (tmp > 15) //在没有输出的情况下读到的数据是15
    iout1_mA = tmp * 5 / 2;
  else
    iout1_mA = 0;
  tmp = 0;

  //读取接口2输出电流
  i2cWriteReg8(SW35XX_ADC_DATA_TYPE, 0x04);

  tmp = i2cReadReg8(SW35XX_ADC_DATA_BUF_H) << 4;
  tmp += i2cReadReg8(SW35XX_ADC_DATA_BUF_L) | 0x0f;
  if (tmp > 15)
    iout2_mA = tmp * 5 / 2;
  else
    iout2_mA = 0;
  tmp = 0;
  //读取pd版本和快充协议
  tmp = i2cReadReg8(SW35XX_FCX_STATUS);
  PDVersion = ((tmp & 0x30) >> 4) + 1;
  fastChargeType = (fastChargeType_t)(tmp & 0x0f);
}
void SW35xx::unlock_i2c_write() {
  i2cWriteReg8(SW35XX_I2C_ENABLE, 0x20);
  i2cWriteReg8(SW35XX_I2C_ENABLE, 0x40);
  i2cWriteReg8(SW35XX_I2C_ENABLE, 0x80);
}

void SW35xx::lock_i2c_write() {
  i2cWriteReg8(SW35XX_I2C_ENABLE, 0x00);
}

void SW35xx::sendPDCmd(SW35xx::PDCmd_t cmd){
  i2cWriteReg8(SW35XX_PD_SRC_REQ, (const uint8_t)cmd);
  i2cWriteReg8(SW35XX_PD_SRC_REQ, (const uint8_t)(cmd | 0x80));
}

void SW35xx::rebroadcastPDO(){
  i2cWriteReg8(SW35XX_I2C_CTRL, 0x03);
}

void SW35xx::setMaxCurrent5A() {
  unlock_i2c_write();
  i2cWriteReg8(SW35XX_PD_CONF1, 0b01100100);
  i2cWriteReg8(SW35XX_PD_CONF2, 0b01100100);
  i2cWriteReg8(SW35XX_PD_CONF3, 0b01100100);
  i2cWriteReg8(SW35XX_PD_CONF4, 0b01100100);
  i2cWriteReg8(SW35XX_PD_CONF6, 0b01100100);
  i2cWriteReg8(SW35XX_PD_CONF7, 0b01100100);
  lock_i2c_write();
}

void SW35xx::setQuickChargeConfiguration(const uint16_t flags,
    const enum QuickChargePowerClass power) {
  /* mask all available bits to avoid setting reserved bits */
  const uint16_t validFlags = flags & QC_CONF_ALL;
  const uint16_t validPower = power & QC_PWR_20V_2;
  const uint8_t conf1 = validFlags;
  const uint8_t conf2 = (validFlags >> 8) | (validPower << 2);

  unlock_i2c_write();
  i2cWriteReg8(SW35XX_QC_CONF1, conf1);
  i2cWriteReg8(SW35XX_QC_CONF2, conf2);
  lock_i2c_write();
}

void SW35xx::setMaxCurrentsFixed(uint32_t ma_5v, uint32_t ma_9v, uint32_t ma_12v, uint32_t ma_15v, uint32_t ma_20v){
  if(ma_5v > 5000) ma_5v = 5000;
  if(ma_9v > 5000) ma_9v = 5000;
  if(ma_12v > 5000) ma_12v = 5000;
  if(ma_15v > 5000) ma_15v = 5000;
  if(ma_20v > 5000) ma_20v = 5000;

  uint8_t tmp = i2cReadReg8(SW35XX_PD_CONF8);

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
  
  unlock_i2c_write();

  i2cWriteReg8(SW35XX_PD_CONF8, tmp);

  i2cWriteReg8(SW35XX_PD_CONF1, ma_5v/50);
  i2cWriteReg8(SW35XX_PD_CONF2, ma_9v/50);
  i2cWriteReg8(SW35XX_PD_CONF3, ma_12v/50);
  i2cWriteReg8(SW35XX_PD_CONF4, ma_15v/50);
  i2cWriteReg8(SW35XX_PD_CONF5, ma_20v/50);

  lock_i2c_write();

}

void SW35xx::setMaxCurrentsPPS(uint32_t ma_pps1, uint32_t ma_pps2) {
  if (ma_pps1 > 5000)
    ma_pps1 = 5000;
  if (ma_pps2 > 5000)
    ma_pps2 = 5000;
  uint8_t tmp = i2cReadReg8(SW35XX_PD_CONF8);

  if(ma_pps1 == 0)
    tmp &= 0b10111111;
  else
    tmp |= 0b01000000;

  if(ma_pps1 == 0)
    tmp &= 0b01111111;
  else
    tmp |= 0b10000000;
  
  
  unlock_i2c_write();

  i2cWriteReg8(SW35XX_PD_CONF8, tmp);

  i2cWriteReg8(SW35XX_PD_CONF6, ma_pps1/50);
  i2cWriteReg8(SW35XX_PD_CONF7, ma_pps2/50);

  lock_i2c_write();
}

} // namespace h1_SW35xx
