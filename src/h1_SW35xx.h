#pragma once
#include <Wire.h>
#include <inttypes.h>

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

class SW35xx {
public:
  enum fastChargeType_t {
    NOT_FAST_CHARGE = 0,
    QC2,
    QC3,
    FCP,
    SCP,
    PD_FIX,
    PD_PPS,
    MTKPE1,
    MTKPE2,
    LVDC,
    SFCP,
    AFC
  };

private:
  TwoWire &_i2c;

public:
  SW35xx(TwoWire &i2c = Wire);
  ~SW35xx();
  void begin();
  void readStatus();
  void readConfig();
  void PDHardReset();
  void rebroadcastPDO();
  void setMaxCurrent5A();

public:
  uint16_t vin_mV;
  uint16_t vout_mV;
  uint16_t iout1_mA;
  uint16_t iout2_mA;
  float temperature;
  enum fastChargeType_t fastChargeType;
  uint8_t PDVersion;

public:
  uint8_t chipVersion;
  bool isBuckOn;
  bool isPort1On;
  bool isPort2On;
  uint8_t pwr_icc;
  bool enableQC3;
  uint16_t vendorID;
  bool enablecustomPD5VCurrent;
  uint16_t customPD5VCurrent_mA;
  bool enablecustomPD9VCurrent;
  uint16_t customPD9VCurrent_mA;
  bool enablecustomPD12VCurrent;
  uint16_t customPD12VCurrent_mA;
  bool enablecustomPD15VCurrent;
  uint16_t customPD15VCurrent_mA;
  uint16_t customPD20VCurrent_mA;
  bool enablecustomPPS0Current;
  uint16_t customPPS0Current_mA;
  bool enablecustomPPS1Current;
  uint16_t customPPS1Current_mA;
  bool enablePPS1;
  bool enablePPS0;
  bool enablePD20V;
  bool enablePD15V;
  bool enablePD12V;
  bool enablePD9V;
  bool enablePDEmarker;
  bool enablePD3;
  bool enablePD5V2A;
  bool enablePD65WEmarker;
  bool enablePort1FastCharge;
  bool enablePort2FastCharge;
  bool enablePD;
  bool enableQC;
  bool enableFCP;
  bool enableSCP;
  bool enableMTKPE;
  bool enableSFCP;
  bool enableAFC;
  uint8_t NonPDMaxVoltage_V;
  bool PPS1CustomMaxVoltageEnable;
  uint8_t PPS1CustomMaxVoltage_V;
  bool PPS2CustomMaxVoltageEnable;
  uint8_t PPS2CustomMaxVoltage_V;
private:
  bool _last_config_read_success;
};

} // namespace h1_SW35xx