#pragma once
#include <Wire.h>
#include <inttypes.h>

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

  enum PDCmd_t {
    HARDRESET = 1
  };

private:
  TwoWire &_i2c;

public:
  SW35xx(TwoWire &i2c = Wire);
  ~SW35xx();
  void begin();
  /**
   * @brief 读取当前充电状态
   * 
   */
  void readStatus();
  /**
   * @brief 发送PD命令
   * 
   * @note 这个芯片似乎可以发送很多种PD命令，但是寄存器文档里只有hardreset. 如果你有PD抓包工具，可以尝试2~15的不同参数，摸索出对应的命令。记得开个pr告诉我!
   */
  void sendPDCmd(PDCmd_t cmd);
  /**
   * @brief 重新广播PDO. 改变最大电流后需要调用此函数或者重新插拔USB线来让设置生效.
   */
  void rebroadcastPDO();
  /**
   * @brief 把PD所有组别的电流设置成5A. 如果你的芯片不是sw3518s请慎重使用
   */
  void setMaxCurrent5A();
   /**
   * @brief 设置固定电压组别的最大输出电流
   * 
   * @param ma_xx 各组别的最大输出电流,单位毫安,最小分度50ma,设为0则关闭
   * @note 5v无法关闭
   */
  void setMaxCurrentsFixed(uint32_t ma_5v, uint32_t ma_9v, uint32_t ma_12v, uint32_t ma_15v, uint32_t ma_20v);
  /**
   * @brief 设置PPS组别的最大输出电流
   * 
   * @param ma_xxx 各组别最大输出电流,单位毫安,最小分度50ma,设为0则关闭
   * @note 注意 PD 配置的最大功率大于 60W 时, pps1 将不会广播 (TODO:datasheet这么写的，没试过)
   *       pps1 的最高电压需要大于 pps0 的最高电压，否则 pps1 不会广播;
   */
  void setMaxCurrentsPPS(uint32_t ma_pps1, uint32_t ma_pps2);
  /**
  //  * @brief 重置最大输出电流
  //  * 
  //  * @note 20v组别的电流不会被重置
  //  */
  // void resetMaxCurrents();
  // /**
  //  * @brief 启用Emarker检测
  //  */
  // void enableEmarker();
  // /**
  //  * @brief 禁用Emarker检测
  //  */
  // void disableEmarker();
public:
  /**
   * @brief 输入电压
   */
  uint16_t vin_mV;
  /**
   * @brief 输出电压
   */
  uint16_t vout_mV;
  /**
   * @brief 输出电流1(type-C)
   */
  uint16_t iout1_mA;
  /**
   * @brief 输出电流2(type-A)
   */
  uint16_t iout2_mA;
  //float temperature;  //TODO
  /**
   * @brief 快充协议
   */
  enum fastChargeType_t fastChargeType;
  /**
   * @brief PD版本(2或者3)
   */
  uint8_t PDVersion;

public:
//TODO

private:
  bool _last_config_read_success;
};

} // namespace h1_SW35xx