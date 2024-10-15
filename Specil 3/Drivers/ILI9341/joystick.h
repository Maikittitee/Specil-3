#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "adc.h"
#include <stdint.h>

// State definitions
typedef enum {
    STATE_MODIFY_PLAYERS,
    STATE_MODIFY_BOTS,
    STATE_IDLE,
	STATE_PLAYGAME
} JoystickState;

// Joystick configuration structure
typedef struct {
    ADC_HandleTypeDef* adc_x;
    ADC_HandleTypeDef* adc_y;
    uint32_t threshold_increase;
    uint32_t threshold_decrease;
    uint32_t mode_switch_high;
    uint32_t mode_switch_low;
} JoystickConfig;

// Function prototypes
void Joystick_Init(JoystickConfig* config);
void Joystick_Update(uint8_t* n_players, uint8_t* n_bots, JoystickState* current_state, int *playGame);
uint32_t Joystick_GetXValue(void);
uint32_t Joystick_GetYValue(void);

#endif // JOYSTICK_H
