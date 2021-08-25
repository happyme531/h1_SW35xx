#include <Arduino.h>
#include <h1_SW35xx.h>
using namespace h1_SW35xx;

SW35xx sw(Wire);

const char *fastChargeType2String(SW35xx::fastChargeType_t fastChargeType) {
  switch (fastChargeType) {
  case SW35xx::NOT_FAST_CHARGE:
    return "Not fast charge";
    break;
  case SW35xx::QC2:
    return "QC2.0";
    break;
  case SW35xx::QC3:
    return "QC3.0";
    break;
  case SW35xx::FCP:
    return "FCP";
    break;
  case SW35xx::SCP:
    return "SCP";
    break;
  case SW35xx::PD_FIX:
    return "PD Fix";
    break;
  case SW35xx::PD_PPS:
    return "PD PPS";
    break;
  case SW35xx::MTKPE1:
    return "MTK PE1.1";
    break;
  case SW35xx::MTKPE2:
    return "MTK PE2.0";
    break;
  case SW35xx::LVDC:
    return "LVDC";
    break;
  case SW35xx::SFCP:
    return "SFCP";
    break;
  case SW35xx::AFC:
    return "AFC";
    break;
  default:
    return "unknown";
    break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(D1, D2); 
  sw.setMaxCurrent5A();
}

void loop() {
  sw.readStatus();
  Serial.println("=======================================");
  Serial.printf("Current input voltage:%dmV\n", sw.vin_mV);
  Serial.printf("Current output voltage:%dmV\n", sw.vout_mV);
  Serial.printf("Current USB-C current:%dmA\r\n", sw.iout_usbc_mA);
  Serial.printf("Current USB-A current:%dmA\r\n", sw.iout_usba_mA);
  Serial.printf("Current fast charge type:%s\n", fastChargeType2String(sw.fastChargeType));
  if (sw.fastChargeType == SW35xx::PD_FIX || sw.fastChargeType == SW35xx::PD_PPS)
    Serial.printf("Current PD version:%d\n", sw.PDVersion);
  Serial.println("=======================================");
  Serial.println("");
  delay(2000);
}
