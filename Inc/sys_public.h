#ifndef SYSPUBLIC_H
#define SYSPUBLIC_H

#include "stdint.h"

volatile uint8_t adcValues[4];
volatile uint16_t selectedDmxChannels[4];
volatile uint8_t selectedDmxChannelsLock[4];

#endif //SYSPUBLIC_H
