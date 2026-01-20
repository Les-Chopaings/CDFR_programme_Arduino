#pragma once

#include <stdarg.h>
#include <string.h>
#include <Arduino.h>

#define type8b uint8_t

int arrayToParameter(type8b* array, int size, const char* format, ...);
int parameterToArray(type8b* array, int size, const char* format, ...);