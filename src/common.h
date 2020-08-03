#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAN_BOXING 1

#ifdef _DEBUG
#define DEBUG_PRINT_CODE 0
#define DEBUG_TRACE_EXECUTION 0
#define DEBUG_STRESS_GC 0
#define DEBUG_LOG_GC 0
#endif

#define UINT8_COUNT (UINT8_MAX + 1)