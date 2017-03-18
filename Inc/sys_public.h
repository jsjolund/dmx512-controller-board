#ifndef SYSPUBLIC_H
#define SYSPUBLIC_H

#include <stdint.h>
#include "buttons.h"
#include "dmx512.h"

volatile uint8_t adcValues[4];
volatile uint16_t selectedDmxChannels[4];
volatile uint8_t selectedDmxInputLock[4];
volatile uint8_t passthroughMode;
uint8_t tmpDmxData[DMX_CHANNELS];

void (*controllerMenuFunction)(enum buttonEnum button, int state);
void (*prevControllerMenuFunction)(enum buttonEnum button, int state);

#endif //SYSPUBLIC_H
