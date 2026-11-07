//
// Created by 27541 on 2026/10/25.
//
#ifndef RM_CAN_TEST_01_BREATH_H
#define RM_CAN_TEST_01_BREATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 一个周期，返回 0-1.0 的呼吸系数
float Breath(uint32_t now);

#ifdef __cplusplus
}
#endif

#endif //RM_CAN_TEST_01_BREATH_H
