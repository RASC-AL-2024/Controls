#ifndef ServoController_h
#define ServoController_h

#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include <ArmConfig.h>
#include "ServoControllerAbstract.h"

#define user_pkt_buf_cap 128

class ServoController : public ServoControllerAbstract {
  private:

    HardwareSerial* DEBUG_SERIAL;

    const int DXL_DIR_PIN = 2;
    const uint8_t BROADCAST_ID = 254;
    const float DYNAMIXEL_PROTOCOL_VERSION = 2.0;

    uint8_t user_pkt_buf[user_pkt_buf_cap];
    double init_servo_positions[N_JOINTS];
    bool initialized = false;

    const uint16_t SR_START_ADDR = 132;
    const uint16_t SR_ADDR_LEN = 4;
    const uint16_t SW_START_ADDR = 116;
    const uint16_t SW_ADDR_LEN = 4;

    typedef struct sr_data{
      int32_t present_position;
    } __attribute__((packed)) sr_data_t;

    typedef struct sw_data{
      int32_t goal_position;
    } __attribute__((packed)) sw_data_t;


    sr_data_t sr_data[N_JOINTS];
    DYNAMIXEL::InfoSyncReadInst_t sr_infos;
    DYNAMIXEL::XELInfoSyncRead_t info_xels_sr[N_JOINTS];

    sw_data_t sw_data[N_JOINTS];
    DYNAMIXEL::InfoSyncWriteInst_t sw_infos;
    DYNAMIXEL::XELInfoSyncWrite_t info_xels_sw[N_JOINTS];

    Dynamixel2Arduino dxl;

    void setOperatingMode();
    void setSyncRead();
    void setSyncWrite();

    void buildSyncWritePacket();
    void buildSyncReadPacket();

    bool checkExecuteState(const double servo_state[]);

  public:
    ServoController(HardwareSerial* software_serial);
    void begin() override;
    void initializeState(const double servo_state[]) override;
    bool executeAction(const int action) override;
    bool executeServoAbs(const double servo_state[]) override;
    bool executeServoDelta(const double servo_delta[]) override;
    void saveState() override;
    void reset() override;
};
#endif
