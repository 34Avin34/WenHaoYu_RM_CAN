//
// Created by 27541 on 2026/10/25.
//

#ifndef RM_CAN_TEST_01_IS_1MS_ELAPSED_H
#define RM_CAN_TEST_01_IS_1MS_ELAPSED_H

#include <stdint.h> //获得uint8_t，uint32_t等关键字

#ifdef __cplusplus
extern "C" {
#endif

/* 每毫秒第一次调用返回 1，其余返回 0。
 * last_ms 由调用方提供，每个任务各用各的变量。 */
uint8_t Is_1ms_Elapsed(uint32_t *last_ms);

#ifdef __cplusplus
}
#endif

#endif //RM_CAN_TEST_01_IS_1MS_ELAPSED_H
