#include "ServoController.h"
#include <ActionMessage.h>
#include <ControlMessage.h>
#include <math.h>

using namespace ControlTableItem;

#define DECIMAL_PRECISION 2
#define MAX_TRIES 5

extern HardwareSerial Serial;

ServoController::ServoController(HardwareSerial* software_serial)
    : DEBUG_SERIAL{software_serial}, dxl{Serial, DXL_DIR_PIN} {}

void ServoController::setOperatingMode() {
  for (uint8_t i = 0; i < N_JOINTS; i++) {
    dxl.torqueOff(DXL_ID_LIST[i]);
    dxl.setOperatingMode(DXL_ID_LIST[i], OP_EXTENDED_POSITION);
    dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID_LIST[i], 100);
  }
  dxl.torqueOn(BROADCAST_ID);
}

void ServoController::setSyncRead() {
  sr_infos.packet.p_buf = user_pkt_buf;
  sr_infos.packet.buf_capacity = user_pkt_buf_cap;
  sr_infos.packet.is_completed = false;
  sr_infos.addr = SR_START_ADDR;
  sr_infos.addr_length = SR_ADDR_LEN;
  sr_infos.p_xels = info_xels_sr;
  sr_infos.xel_count = 0;

  for (uint8_t i = 0; i < N_JOINTS; i++) {
    info_xels_sr[i].id = DXL_ID_LIST[i];
    info_xels_sr[i].p_recv_buf = (uint8_t*)&sr_data[i];
    sr_infos.xel_count++;
  }
  sr_infos.is_info_changed = true;
}

void ServoController::setSyncWrite() {
  sw_infos.packet.p_buf = nullptr;
  sw_infos.packet.is_completed = false;
  sw_infos.addr = SW_START_ADDR;
  sw_infos.addr_length = SW_ADDR_LEN;
  sw_infos.p_xels = info_xels_sw;
  sw_infos.xel_count = 0;

  for (uint8_t i = 0; i < N_JOINTS; i++) {
    info_xels_sw[i].id = DXL_ID_LIST[i];
    info_xels_sw[i].p_data = (uint8_t*)&sw_data[i].goal_position;
    sw_infos.xel_count++;
  }
  sw_infos.is_info_changed = true;
}

void ServoController::begin() {
  pinMode(LED_BUILTIN, OUTPUT);
#ifdef DEBUG
  DEBUG_SERIAL->println("in debug mode!");
#endif
  dxl.begin(57600);
  dxl.setPortProtocolVersion(DYNAMIXEL_PROTOCOL_VERSION);

  setOperatingMode();
  setSyncRead();
  setSyncWrite();

  buildSyncReadPacket();
  for (uint8_t i = 0; i < N_JOINTS; i++) {
    sw_data[i].goal_position = sr_data[i].present_position;
  }

#ifdef IS_INITIALIZED
  initializeState(INIT_POSITIONS);
#endif
}

void ServoController::initializeState(const double servo_state[]) {
  for (int i = 0; i < N_JOINTS; i++) {
    init_servo_positions[i] = servo_state[i];
  }
  initialized = true;
#ifdef DEBUG
  DEBUG_SERIAL->println("servo position initialized");
#endif
}

void ServoController::saveState() {
  buildSyncReadPacket();
  for (int i = 0; i < N_JOINTS; i++) {
    init_servo_positions[i] = sr_data[i].present_position;
  }
#ifdef DEBUG
  DEBUG_SERIAL->println("servo position initialized");
  for (int i = 0; i < N_JOINTS; i++) {
    DEBUG_SERIAL->print(init_servo_positions[i]);
    DEBUG_SERIAL->print(" ");
  }
  DEBUG_SERIAL->println("");
#endif
  initialized = true;
}

void ServoController::reset() {
  if (!initialized) {
#ifdef DEBUG
    DEBUG_SERIAL->println("not initialized, can't reset");
#endif
    return;
  }

#ifdef DEBUG
  DEBUG_SERIAL->println("reseting position!");
#endif
  double* empty_position = new double[N_JOINTS];
  for (int i = 0; i < N_JOINTS; i++) {
    empty_position[i] = 0;
  }
  executeServoAbs(empty_position);
  delete[] empty_position;
}

void ServoController::buildSyncWritePacket() {
  bool write_result = dxl.syncWrite(&sw_infos);
#ifdef DEBUG
  if (write_result == true) {
    DEBUG_SERIAL->println("[SyncWrite] Success");
    for (uint8_t i = 0; i < sw_infos.xel_count; i++) {
      DEBUG_SERIAL->print("  ID: ");
      DEBUG_SERIAL->println(sw_infos.p_xels[i].id);
      DEBUG_SERIAL->print("\t Goal Position: ");
      DEBUG_SERIAL->println(sw_data[i].goal_position);
    }
  } else {
    DEBUG_SERIAL->print("[SyncWrite] Fail, Lib error code: ");
    DEBUG_SERIAL->print(dxl.getLastLibErrCode());
  }
  DEBUG_SERIAL->println();
#endif // DEBUG
}

void ServoController::buildSyncReadPacket() {
  int recv_cnt = dxl.syncRead(&sr_infos);
#ifdef DEBUG
  if (recv_cnt > 0) {
    DEBUG_SERIAL->print("[SyncRead] Success, Received ID Count: ");
    DEBUG_SERIAL->println(recv_cnt);
    for (uint8_t i = 0; i < recv_cnt; i++) {
      DEBUG_SERIAL->print("  ID: ");
      DEBUG_SERIAL->print(sr_infos.p_xels[i].id);
      DEBUG_SERIAL->print(", Error: ");
      DEBUG_SERIAL->println(sr_infos.p_xels[i].error);
      DEBUG_SERIAL->print("\t Present Position: ");
      DEBUG_SERIAL->println(sr_data[i].present_position);
    }
  } else {
    DEBUG_SERIAL->print("[SyncRead] Fail, Lib error code: ");
    DEBUG_SERIAL->println(dxl.getLastLibErrCode());
  }
  DEBUG_SERIAL->println("=======================================================");

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
}

bool ServoController::executeAction(const int action) {
  uint8_t ping_id;
  if (!pingIdFromAction(action, ping_id)) {
#ifdef DEBUG
    DEBUG_SERIAL->print("the action: ");
    DEBUG_SERIAL->print(action);
    DEBUG_SERIAL->println(" is not valid.");
#endif
    return false;
  }

  bool success = dxl.ping(ping_id);
  for (int i = 0; i < MAX_TRIES && !success; i++) {
    success = dxl.ping(ping_id);
  }
#ifdef DEBUG
  DEBUG_SERIAL->print("action success: ");
  DEBUG_SERIAL->println(success);
#endif
  return success;
}

int32_t servo_radians_to_raw(float rad) {
  float two_pi = 2 * M_PI;
  int32_t decimal_precision = (int32_t)(pow(10, DECIMAL_PRECISION));
  int32_t adjusted_rad = (int32_t)(GEAR_RATIO * (rad * decimal_precision));

  return (int32_t)(adjusted_rad * (int32_t)(4095.0 / two_pi)) / decimal_precision;
}

float servo_raw_to_radians(int raw) { return raw * (6.28318531 / 4095.0); }

bool isCloseEnough(const int32_t& a, const int32_t& b) { return abs(a - b) < 0.5; }

bool ServoController::checkExecuteState(const double servo_state[]) {
  for (uint8_t i = 0; i < N_JOINTS; i++) {
    if (!isCloseEnough(servo_state[i], sr_data[i].present_position)) {
      return false;
    }
  }
  return true;
}

bool ServoController::executeServoAbs(const double servo_state[]) {
  if (!initialized) {
#ifdef DEBUG
    DEBUG_SERIAL->println("servo controller not initialized!");
#endif
    return false;
  }
  double deltas[N_JOINTS];
  for (int i = 0; i < N_JOINTS; i++) {
    sw_data[i].goal_position = servo_radians_to_raw(servo_state[i] + init_servo_positions[i]);
  }
#ifdef DEBUG
  DEBUG_SERIAL->println("sent servo abs command!");
#endif
  sw_infos.is_info_changed = true;
  buildSyncWritePacket();
  return true;
}

bool ServoController::executeServoDelta(const double servo_delta[]) {
  for (uint8_t i = 0; i < N_JOINTS; i++) {
    sw_data[i].goal_position += servo_radians_to_raw(servo_delta[i]);
  }
#ifdef DEBUG
  DEBUG_SERIAL->println("sent servo delta command!");
#endif
  sw_infos.is_info_changed = true;
  buildSyncWritePacket();
  return true;
}
