//
// Created by 27541 on 2026/10/25.
//

#ifndef RM_CAN_TEST_01_MOTORCONTROL_H
#define RM_CAN_TEST_01_MOTORCONTROL_H
#include <stdint.h> //依旧uint8_t，uint32_t

#ifdef __cplusplus
extern "C" {
#endif

// 在 1ms 门闸里调用：根据当前模式发一帧 CAN
void MotorControl(uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif //RM_CAN_TEST_01_MOTORCONTROL_H
