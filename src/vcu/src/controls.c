#include "controls.h"

#define HUMAN_READABLE true

static bool enabled = false;
static uint16_t torque_command = 0;

void enable_controls(void) {
  enabled = true;
  torque_command = 0;
  unlock_brake_valve();
}

void disable_controls(void) {
  enabled = false;
  torque_command = 0;

  set_brake_valve(false);
  lock_brake_valve();
}

bool get_controls_enabled(void) {
  return enabled;
}

void execute_controls(void) {
  if (!enabled) return;

  torque_command = get_torque();
  sendTorqueCmdMsg(torque_command);

  // Control regen brake valve:
  bool brake_valve_state = REGEN && get_pascals(pedalbox.REAR_BRAKE) < RG_REAR_BRAKE_THRESH;
  set_brake_valve(brake_valve_state);

  int32_t regen_torque;
  if (brake_valve_state) {
    regen_torque = get_regen_torque();
  } else {
    regen_torque = 0;
  }
}

static int16_t get_torque(void) {
  auto accel = pedalbox_avg(accel);

  if (accel < PEDALBOX_ACCEL_RELEASE) return 0;

  int16_t raw_torque = MAX_TORQUE * (accel - PEDALBOX_ACCEL_RELEASE) /
                       (MAX_ACCEL_VAL - PEDALBOX_ACCEL_RELEASE);
  int16_t power_limited_torque = get_pwr_limited_torque(raw_torque);

  if (power_limited_torque > raw_torque) power_limited_torque = raw_torque;

  return power_limited_torque;
}

static int32_t get_regen_torque() {
  int32_t regen_torque = 0;

  if ((mc_readings.speed * -1 > RG_MOTOR_SPEED_THRESH) &&
      (cs_readings.V_bus < RG_BATTERY_VOLTAGE_MAX_THRESH) &&
      (get_pascals(pedalbox.FRONT_BRAKE) > RG_FRONT_BRAKE_THRESH) &&
      // check car speed
      true) {
    int32_t kilo_pascals = get_pascals(pedalbox.FRONT_BRAKE) / 1000;
    // Because we already divded by 1000 by using kilo_pascals instead of
    // pascals, we only need to divde by 10^4, not 10^7 for RG_10_7_K
    // And by dividing by 10^3 instead of 10^4, we are multiplying by 10 to get
    // dNm instead of Nm
    regen_torque = RG_10_7_K * kilo_pascals * (100 - RG_cBB_ef) /(RG_cBB_ef * 1000);
    if (regen_torque > RG_TORQUE_COMMAND_MAX) {
      regen_torque = RG_TORQUE_COMMAND_MAX;
    }
  }

  return -1 * regen_torque;
}

static int16_t get_launch_control_torque() {
  static uint16_t last_torque  = 0;
  static uint32_t LC_error_sum = 0;

  // Launch control

  /*uint16_t launch_control_torque;
     if (true) {
     // If we are in the first few miliseconds, send max torque
     if (msTicks < LC_TIME_OFFSET) {
      if (raw_torque - last_torque > LC_SLEW_RATE_CAP) {
        launch_control_torque = last_torque + LC_SLEW_RATE_CAP
      } else launch_control_torque = raw_torque;
     } else {
      // Calculate setpoint
      uint16_t setpoint = mc_readings.speed * LC_TARGET_SLIP_RATIO;

      // Update PI controller
      uint16_t error =
     }
     } else {
     launch_control_torque = raw_torque;
     }*/
  return 0;
}

static int16_t get_pwr_limited_torque(int16_t raw_torque) {
  static int32_t PL_I_sum = 0;

  // I keep making unit mistakes so I've added a bunch of comments with untis
  uint32_t power_limited_torque;                     // Units: 10 * Nm
  int32_t  error = cs_readings.power - PL_THRESHOLD; // Units: W

  PL_I_sum += error * can0_MCCommand_period;         // Units: mJ += W * ms

  // Limit PL_I_sum
  if (PL_I_sum < 0) PL_I_sum = 0;                    // Units: mJ

  if (PL_I_sum > PL_I_CAP) PL_I_sum = PL_I_CAP;      // Units: mJ

  if (error < 0) error = 0;                          // Units: W

  // Speed starts negative becuase of direction
  int32_t mc_speed = mc_readings.speed * -1;         // Units: RPM

  // Units: rad/s = (rotations / minute) * (2pi rad / rotation) * (1 minute / 60
  // secs)
  uint32_t omega = mc_speed * (2 * 314) / (60 * 100);

  // Multiplying by ten to improve accuracy and convert units
  // Units: dNm  = (10 dNM / Nm) * Nm
  uint32_t P_torque = 10 * error  * PL_KP;

  // Units:  1000 dNM = (10 dNM / Nm) * (mNm)
  uint32_t I_torque = 10 * PL_KI * PL_I_sum;

  // PL_I sum is integrated with respect to ms
  // Both need to be divided by omega, but only PI needs to be dvided by 1000,
  // so multiply the P portion by 1000
  // dNm = (dNm * 1000 + 1000 dNM) / (1000 * rad/s)
  uint32_t torque_offset = (P_torque * 1000 + I_torque) / (1000 * omega);
  uint32_t PI_torque;                       // Units: dNm

  if (torque_offset > raw_torque) {
    PI_torque = 0;                          // Units: dNm
  } else {
    PI_torque = raw_torque - torque_offset; // Units: dNm = dNm - dNm
  }

  // Mechanical limit
  // Units: dNm = (10 dNM / Nm) * (Nm/s) / (rad/s)
  uint32_t target_torque = 10 * PL_THRESHOLD / omega;

  if ((omega != 0) && (PI_torque > target_torque)) { // Units: rad/s && dNm
    power_limited_torque = target_torque;            // dNm
  } else {
    power_limited_torque = PI_torque;                // Units: dNm
  }

  /*if (HUMAN_READABLE) {
     printf("POWER: %d, RAW TORQUE: %d, TORQUE OFFSET: %d, COMMANDED TORQUE: %d,
        omega: %d, motor speed: %d, error: %d, I_torque: %d, I-sum:%d\r\n",
        cs_readings.power, raw_torque, torque_offset, power_limited_torque,
        omega, mc_speed, error, I_torque, PL_I_sum);
     } else {
     printf("power,%d,%d\r\n", cs_readings.power, HAL_GetTick());
     printf("raw_torque,%d,%d\r\n", raw_torque, HAL_GetTick());
     printf("torque_offset,%d,%d\r\n", torque_offset, HAL_GetTick());
     printf("power_limited_torque,%d,%d\r\n", power_limited_torque,
        HAL_GetTick());
     printf("omega,%d,%d\r\n",omega, HAL_GetTick());
     printf("motor_speed,%d,%d\r\n",mc_speed, HAL_GetTick());
     printf("error,%d,%d\r\n",error, HAL_GetTick());
     }

     pl.raw_torque = raw_torque;
     pl.power_limited_torque = power_limited_torque;
     pl.omega = omega;
     pl.error = error;
     pl.PI_torque = PI_torque;
     pl.torque_offset = torque_offset;
     pl.I_sum = PL_I_sum;
     pl.P_torque = P_torque;
     pl.I_torque = I_torque;*/
  return power_limited_torque;
}